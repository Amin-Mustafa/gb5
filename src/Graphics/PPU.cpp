#include "../../include/Graphics/PPU.h"
#include "../../include/Memory/MMU.h"
#include "../../include/Memory/InterruptController.h"
#include "../../include/Graphics/Spaces.h"
#include "../../include/Arithmetic.h"
#include <iostream>
#include <format>

constexpr unsigned int SCANLINE_START = -1; 
constexpr unsigned int SCANLINE_END = 455;
constexpr unsigned int VBLANK_LINES = 10;

uint8_t display_color(uint8_t palette, uint8_t px);

PPU::PPU(MMU& mmu, InterruptController& interrupt_controller)
    :vram{mmu},
     oam{mmu},
     regs{mmu}, 
     bg_fetcher{vram, regs, bg_fifo},
     ic{interrupt_controller},
     current_state{ pixel_transfer }
     {
        bg_fetcher.set_position(scanline_x, regs.ly);
     }

bool PPU::window_triggered() const {
    return  (!in_window)            &&
            (lcdc_win_enable(regs)) &&
            (regs.ly == regs.wy)    &&
            (scanline_x >= regs.wx - 7);           
}    

void PPU::pixel_transfer() {
    //check if reached window
    if(window_triggered()) {
        in_window = true;
        bg_fetcher.set_mode(PixelFetcher::Mode::WIN_FETCH);
        //transform coordinates into window space
        bg_fetcher.set_position(scanline_x, regs.ly);
    }

    bg_fetcher.tick();

    // if no pixels ready -> nothing to do
    if(bg_fifo.empty()) {
        return; 
    } 
    
    // otherwise start drawing
    // take a pixel from the FIFO
    uint8_t px = bg_fifo.pop();

    int pos = scanline_x - regs.scx % 8;
    scanline_x++;

    // pos < 0 means there are pixels in the FIFO "behind" the screen
    if(pos >= 0 && screen != nullptr) {
        //reached left edge of screen
        uint8_t display_px = display_color(regs.bgp, px);
        screen->blit(display_px, pos, regs.ly);
    }
    
    if(pos >= screen->width() - 1) {
        //end of line
        current_state = h_blank;
    }
}

void PPU::h_blank() {
    if(cycles < SCANLINE_END) {
        //do nothing until dot number 455
        return;
    }

    //prepare to draw next scanline 
    regs.ly++;
    scanline_x = 0;
    bg_fetcher.set_mode(PixelFetcher::Mode::BG_FETCH);
    bg_fetcher.set_position(scanline_x, regs.ly);
    bg_fifo.clear();

    in_window = false;
    
    cycles = SCANLINE_START;
    current_state = pixel_transfer; //TODO: OAM SCAN

    if(regs.ly == screen->height()) {
        current_state = v_blank;
    }
}

void PPU::v_blank() {
    if(cycles < SCANLINE_END) {
        //do nothing until end of scanline
        return;
    }
    regs.ly++;
    cycles = SCANLINE_START;

    if(regs.ly == screen->height() + VBLANK_LINES) {
        regs.ly = 0;
        bg_fetcher.set_mode(PixelFetcher::Mode::BG_FETCH);
        bg_fetcher.set_position(scanline_x, regs.ly);
        bg_fifo.clear();

        current_state = pixel_transfer;
    }
}

void PPU::print_state() {
    std::cout << "cycle:" << cycles << std::endl;
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
    std::cout << "Pixel FIFO: ";
    bg_fifo.print();
}

uint8_t display_color(uint8_t palette, uint8_t px) {
    if(px > 0x03) return 0;
    return (palette >> (2*px)) & (uint8_t)3;
}