#include "Graphics/PPU.h"
#include "Memory/MMU.h"
#include "Memory/Bus.h"
#include "Memory/InterruptController.h"
#include "Memory/Spaces.h"
#include "Graphics/LCD.h"
#include "Graphics/Sprite.h"
#include "Arithmetic.h"
#include <iostream>
#include <format>

enum StateDots {
    SCANLINE_START = 0, 
    SCANLINE_END = 455,
    OAM_SCAN_START = 0,
    OAM_SCAN_END = 79,
    VBLANK_START = 0,
    VBLANK_END = 4559,
    PIXEL_TRANSFER_START = 80,
};
enum Position {
    VBLANK_LINES = 10,
    BASE_SPR_HEIGHT = 8,
    SCREEN_Y_OFFSET = 16,
    SCREEN_X_OFFSET = 8,
};
constexpr int SPRITE_BYTES = 4;

uint8_t display_color(uint8_t palette, uint8_t px);
uint8_t mix_pixel(uint8_t bg_px, SpritePixel spr_px, const PPURegs& regs);
std::string ppu_state_to_str(PPU::State state);

PPU::PPU(Bus& bus, MMU& mmu, InterruptController& interrupt_controller)
    :vram{mmu},
     oam{mmu},
     regs{bus, mmu}, 
     mmu{mmu},
     bg_fetcher{vram, regs, bg_fifo},
     spr_fetcher{vram, regs, spr_fifo},
     ic{interrupt_controller},
     current_state{ oam_scan }
     {
        bg_fetcher.set_position(scanline_x, regs.ly);
        bus.connect(*this);
        bus.oam_dma_dest = &oam;
     }

void PPU::tick() {
    //do not tick if PPU switched off
    if(!LCDC::lcd_enable(regs)) {
        regs.ly = 0;
        scanline_x = 0;
        current_state = oam_scan;
        cycles = OAM_SCAN_START;
        vram.unblock(mmu);
        oam.unblock(mmu);
        STAT::set_mode(regs, STAT::Mode::MODE_0);
        return;
    }

    //execute current state function
    (this->*current_state)();

    //check if stat trigger executed
    if(stat_trigger.rising_edge(STAT::stat_line(regs))) {
        ic.request(Interrupt::LCD);
    }
    cycles++;
}

void PPU::check_window_transition() {
    bool window_triggered = (LCDC::win_enable(regs)) &&
                            (regs.ly >= regs.wy)    &&
                            ((int)scanline_x >= (int)regs.wx - 7);    

    if(!in_window && window_triggered) {
        in_window = true;
        //transform coordinates into window space
        bg_fetcher.set_mode(PixelFetcher::Mode::WIN_FETCH);
        bg_fetcher.set_position(scanline_x, regs.ly);
    }
}


uint8_t PPU::sprite_triggered() const {
    if(!LCDC::obj_enable(regs)) {
        return spr_buf.count();
    }
    for(int i = 0; i < spr_buf.count(); ++i) {
        if( scanline_x == spr_buf.at(i).x() - 2*SCREEN_X_OFFSET ) {
            return i;
        }
    }
    return spr_buf.count();
}

void PPU::go_next_scanline(){
    regs.ly = (regs.ly+1) % (screen->height() + VBLANK_LINES);
    STAT::update_lyc_flag(regs);
}

void PPU::prep_scanline() {
    //start from the left
    scanline_x = 0;
    in_window = false;

    //reset fetch pipeline
    bg_fetcher.set_mode(PixelFetcher::Mode::BG_FETCH);
    bg_fetcher.set_position(scanline_x, regs.ly);
    
    bg_fifo.clear();
    spr_fifo.clear();
    spr_fetcher.clear_queue();

    check_window_transition();
    
    //check visible sprites "behind" the screen
    for(int i = 0; i < spr_buf.count(); ++i) {
        const Sprite& spr = spr_buf.at(i);
        if(spr.x() > 0 && spr.x() <= 2*SCREEN_X_OFFSET) {
            bg_fetcher.request_stop();
            spr_fetcher.queue_sprite(spr);
        }
    }
}

void PPU::advance_scanline() {
    //increment scanline x and check win/spr triggers
    scanline_x++;

    check_window_transition();

    // check if reached sprite
    uint8_t index = sprite_triggered();
    if (index != spr_buf.count())
    {
        bg_fetcher.request_stop(); // stop after completing current step
        spr_fetcher.queue_sprite(spr_buf.at(index));
    }
}

void PPU::oam_scan() {  
    //AKA mode 2
    //std::cout << "STATE: OAM SCAN\n";
    if(cycles == OAM_SCAN_START) {
        STAT::set_mode(regs, STAT::MODE_2);
        oam.block(mmu);
        spr_buf.clear();   //prepare sprite buffer
        oam_counter = 0;
    }
    if((cycles % 2) == 0) {
        //every 2 dots
        if(!spr_buf.full() && LCDC::obj_enable(regs)) {
            //current sprite
            Sprite spr = oam.sprite_at(Space::OAM_START + oam_counter*SPRITE_BYTES);

            //sprite height mode at current dot
            uint8_t spr_height = BASE_SPR_HEIGHT + BASE_SPR_HEIGHT*LCDC::obj_size(regs);

            //position of scanline wrt to sprite
            int pos = regs.ly - (spr.y() - SCREEN_Y_OFFSET);
            
            if(pos >= 0 && pos < spr_height) {
                spr_buf.push_sprite(spr);
            }
        }
        oam_counter++;
    }
    if(cycles == OAM_SCAN_END) {
        current_state = pixel_transfer;
        prep_scanline();
    }
}

void PPU::pixel_transfer() {
    //AKA mode 3
    //std::cout << "STATE: PIXEL TRANSFER\n";
    if(cycles == PIXEL_TRANSFER_START) {
        STAT::set_mode(regs, STAT::MODE_3);
        vram.block(mmu);
    }
    
    if(!spr_fetcher.active()) {
        bg_fetcher.tick();
        if(!bg_fetcher.active()) {
            //bg fetcher done using VRAM bus, 
            //switch to spr fetcher
            spr_fetcher.start();
            if(!spr_fetcher.active()) {
                bg_fetcher.start();
            }
        }
    } else {
        spr_fetcher.tick();
        if(!spr_fetcher.active()) {
            //spr fetcher done with VRAM bus
            bg_fetcher.start();
        }
    }

    if(bg_fifo.empty()) {
        return;
    }

    int pos = scanline_x - regs.scx % 8;
    uint8_t bg_px = bg_fifo.pop();
    SpritePixel spr_px;
    if(!spr_fifo.empty() && (scanline_x == spr_fifo.front().x - SCREEN_X_OFFSET)) {
        spr_px = spr_fifo.pop();
    }
    uint8_t display_px = mix_pixel(bg_px, spr_px, regs);

    // pos < 0 means there are pixels in the FIFO "behind" the screen
    if(pos >= 0 && screen != nullptr) {
        //reached left edge of screen
        screen->blit(display_px, pos, regs.ly);
    }
    
    advance_scanline();

    if(scanline_x >= screen->width()) {
        //end of line
        current_state = h_blank;
        vram.unblock(mmu);
        oam.unblock(mmu);
    }
}

void PPU::h_blank() {
    //AKA mode 0
    //std::cout << "STATE: H BLANK\n";
    if(scanline_x == screen->width()) {
        //first dot of HBLANK
        STAT::set_mode(regs, STAT::MODE_0);
    }
    if(cycles < SCANLINE_END) {
        //do nothing until dot number 455
        return;
    }

    //start next oam scan
    go_next_scanline();
    cycles = OAM_SCAN_START - 1;
    current_state = oam_scan;

    if(regs.ly == screen->height()) {
        //if next scanline is off-screen
        cycles = VBLANK_START - 1;
        current_state = v_blank;
    }
}

void PPU::v_blank() {
    //AKA mode 1
    //std::cout <<"STATE: VBLANK\n";
    if(cycles == VBLANK_START) {
        //first dot of V BLANK
        STAT::set_mode(regs, STAT::MODE_1);
        ic.request(Interrupt::VBLANK);
        return;
    }

    if(cycles % (SCANLINE_END+1) == 0) {
        go_next_scanline();

        if(regs.ly == 0) {
            //looped back to start of screen
            cycles = OAM_SCAN_START - 1;
            current_state = oam_scan;
        }
    }
}

uint8_t display_color(uint8_t palette, uint8_t px) {
    if(px > 0x03) return 0;
    return (palette >> (2*px)) & (uint8_t)3;
}

uint8_t mix_pixel(uint8_t bg_px, SpritePixel spr_px, const PPURegs& regs) {
    Sprite::Priority prio = spr_px.priority;
    Sprite::Palette pal = spr_px.palette;
    uint8_t color = 0;

    if(!spr_px.color || !LCDC::obj_enable(regs)) {
        color = display_color(regs.bgp, bg_px);
    } else if(prio == Sprite::Priority::BACK && bg_px) {
        color = display_color(regs.bgp, bg_px);
    } else {
        switch(pal) {
            case Sprite::Palette::OBP0:
                color = display_color(regs.obp_0, spr_px.color);
                break;
            case Sprite::Palette::OBP1:
                color = display_color(regs.obp_1, spr_px.color);
                break;
        }
    }

    return color;
}