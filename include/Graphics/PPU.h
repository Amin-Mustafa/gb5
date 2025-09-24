#ifndef PPU_H
#define PPU_H

#include <memory>
#include "VRAM.h"
#include "OAM.h"
#include "PPURegs.h"
#include "../../include/Memory/MemoryRegion.h"
#include "PixelFetcher.h"
#include "LCD.h"
#include "FIFO.h"

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
    bool window_triggered() const;
    
    unsigned int cycles; //cycles in current scanline (0 - 455) 
    InterruptController& ic;

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
    void tick() {
        (this->*current_state)();
        cycles++;
    }

    void print_state();
};  

#endif