#include "../../include/Graphics/PPU.h"
#include "../../include/Memory/MMU.h"
#include "../../include/Memory/InterruptController.h"
#include "../../include/Memory/Spaces.h"
#include "../../include/Graphics/LCD.h"
#include "../../include/Graphics/Sprite.h"
#include "../../include/Arithmetic.h"
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

PPU::PPU(MMU& mmu, InterruptController& interrupt_controller)
    :vram{mmu},
     oam{mmu},
     regs{mmu}, 
     mmu{mmu},
     bg_fetcher{vram, regs, bg_fifo},
     spr_fetcher{vram, regs, spr_fifo},
     ic{interrupt_controller},
     current_state{ oam_scan },
     curr_state_enum{State::OAM_SCAN}
     {
        bg_fetcher.set_position(scanline_x, regs.ly);
     }

void PPU::tick() {
    //do not tick if PPU switched off
    if(!LCDC::lcd_enable(regs)) {
        regs.ly = 0;
        scanline_x = 0;
        current_state = oam_scan;
        curr_state_enum = State::OAM_SCAN;
        cycles = OAM_SCAN_START;
        vram.accessible = true;
        oam.accessible = true;
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

bool PPU::window_triggered() const {
    return  (!in_window)            &&
            (LCDC::win_enable(regs)) &&
            (regs.ly == regs.wy)    &&
            (scanline_x >= regs.wx - 7);           
}    

uint8_t PPU::sprite_triggered() const {
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

    //reset fetch pipeline
    bg_fetcher.set_mode(PixelFetcher::Mode::BG_FETCH);
    bg_fetcher.set_position(scanline_x, regs.ly);
    bg_fifo.clear();
    spr_fifo.clear();
    spr_fetcher.clear_queue();

    //check win/spr triggers
    if(window_triggered()) {
        in_window = true;
        bg_fetcher.set_mode(PixelFetcher::Mode::WIN_FETCH);
        //transform coordinates into window space
        bg_fetcher.set_position(scanline_x, regs.ly);
    }
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

    // check if reached window
    if (window_triggered())
    {
        in_window = true;
        bg_fetcher.set_mode(PixelFetcher::Mode::WIN_FETCH);
        // transform coordinates into window space
        bg_fetcher.set_position(scanline_x, regs.ly);
    }

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
        oam.accessible = false;
        spr_buf.clear();   //prepare sprite buffer
        oam_counter = 0;
    }
    if((cycles % 2) == 0) {
        //every 2 dots
        if(!spr_buf.full()) {
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
        curr_state_enum = State::PIXEL_TRANSFER;
        prep_scanline();
    }
}

void PPU::pixel_transfer() {
    //AKA mode 3
    //std::cout << "STATE: PIXEL TRANSFER\n";
    if(cycles == PIXEL_TRANSFER_START) {
        STAT::set_mode(regs, STAT::MODE_3);
        vram.accessible = false;
    }
    
    if(!spr_fetcher.active()) {
        bg_fetcher.tick();
        if(!bg_fetcher.active()) {
            //bg fetcher done using VRAM bus, 
            //switch to spr fetcher
            spr_fetcher.start();
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
        curr_state_enum = State::H_BLANK;
        vram.accessible = true;
        oam.accessible = true;
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
    curr_state_enum = State::OAM_SCAN; 

    if(regs.ly == screen->height()) {
        //if next scanline is off-screen
        cycles = VBLANK_START - 1;
        current_state = v_blank;
        curr_state_enum = State::V_BLANK;
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
            curr_state_enum = State::OAM_SCAN;
        }
    }
}

void PPU::print_state() {
    std::cout << "cycle:" << cycles << std::endl;
    std::cout << "STATE: " << ppu_state_to_str(curr_state_enum) << std::endl;
    std::cout << "X:" << (int)(scanline_x - regs.scx % 8) << ' '; 
    std::cout << std::format("LCDC:{:02x} ", regs.lcdc);
    std::cout << std::format("STAT:{:02x} ", regs.stat);
    std::cout << std::format("SCY:{:02x} ", regs.scy);
    std::cout << std::format("SCX:{:02x} ", regs.scx);
    std::cout << std::format("LY:{:02x} ", regs.ly);
    std::cout << std::format("LYC:{:02x} ", regs.lyc);
    std::cout << std::format("DMA:{:02x} ", regs.dma);
    std::cout << std::format("BGP:{:02x} ", regs.bgp);
    std::cout << std::format("OBP0:{:02x} ", regs.obp_0);
    std::cout << std::format("OBP1:{:02x} ", regs.obp_1);
    std::cout << std::format("WY:{:02x} ", regs.wy);
    std::cout << std::format("WX:{:02x} ", regs.wx);
    std::cout << std::endl;

    std::cout << "BG Fetcher: " << '\n';
    bg_fetcher.print_state();
    std::cout << "BG FIFO: ";
    bg_fifo.print();
    std::cout << "SPR Fetcher: " << '\n';
    spr_fetcher.print_state();
    std::cout << "SPR BUFFER: " 
              << "Count: " << (int)spr_buf.count()
              << " Head: ";
    if(spr_buf.count()) {
    std::cout << "X:" << (int)spr_buf.at(0).x()
              << ", Y:" << (int)spr_buf.at(0).y()
              << ", Index:" << (int)spr_buf.at(0).index();
    } else {
        std::cout << "NULL";
    }
    std::cout << std::endl;
}

uint8_t display_color(uint8_t palette, uint8_t px) {
    if(px > 0x03) return 0;
    return (palette >> (2*px)) & (uint8_t)3;
}

uint8_t mix_pixel(uint8_t bg_px, SpritePixel spr_px, const PPURegs& regs) {
    Sprite::Priority prio = spr_px.priority;
    Sprite::Palette pal = spr_px.palette;
    uint8_t color = 0;

    if(!spr_px.color) {
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

std::string ppu_state_to_str(PPU::State state) {
    switch(state) {
        case PPU::State::OAM_SCAN:
            return "OAM SCAN";
        case PPU::State::PIXEL_TRANSFER:
            return "PIXEL TRANSFER";
        case PPU::State::H_BLANK:
            return "H BLANK";
        case PPU::State::V_BLANK:
            return "V BLANK";
    }
}