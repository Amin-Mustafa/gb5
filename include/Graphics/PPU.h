#ifndef PPU_H
#define PPU_H

#include <memory>
#include "VRAM.h"
#include "OAM.h"
#include "PPURegs.h"
#include "../../include/Memory/MemoryRegion.h"
#include "PixelFetcher.h"
#include "FIFO.h"
#include "../include/EdgeDetector.h"

class MMU;
class InterruptController;
class LCD;

class PPU {
private:
    //memory
    VRAM vram;
    OAM oam;
    PPURegs regs;

    //drawing facilities
    PixelFetcher bg_fetcher;
    FIFO bg_fifo;
    int scanline_x; 
    LCD* screen;
    bool in_window;
    void go_next_scanline();
    bool window_triggered() const;
    
    unsigned int cycles; //cycles in current scanline (0 - 455) 
    InterruptController& ic;
    EdgeDetector stat_trigger;

    //PPU states
    void oam_scan();
    void pixel_transfer();
    void h_blank();
    void v_blank();

public:
    PPU(MMU& mmu, InterruptController& interrupt_controller);
    
    void connect_display(LCD* display) {
        screen = display;
    }

    using StateFunction = void (PPU::*)();  //pointer to state function
    StateFunction current_state;
    //PPU is clocked in t-states (4 t-state = 1 m-cycle)
    void tick();

    void print_state();
};  

#endif