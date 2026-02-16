#ifndef PPU_H
#define PPU_H

#include <memory>
#include "VRAM.h"
#include "OAM.h"
#include "PPURegs.h"
#include "PixelFetcher.h"
#include "RingBuffer.h"
#include "SpriteFetcher.h"  
#include "../include/EdgeDetector.h"
#include <string>

class MMU;
class InterruptController;
class LCD;
class Bus;

class PPU {
private:
    //memory
    VRAM vram;
    OAM oam;
    PPURegs regs;
    MMU& mmu;

    //drawing facilities
    PixelFetcher bg_fetcher;
    SpriteFetcher spr_fetcher;
    SpriteBuffer spr_buf;
    BgFifo bg_fifo;
    SprFifo spr_fifo;
    uint8_t scanline_x;     //position on screen (0-159)
    uint8_t oam_counter;
    LCD* screen;
    bool in_window;
    void check_window_transition();
    uint8_t sprite_triggered() const;

    //scanline
    void go_next_scanline();
    void prep_scanline();
    void advance_scanline();

    int cycles;
    InterruptController& ic;
    EdgeDetector stat_trigger;

    //PPU states
    void oam_scan();
    void pixel_transfer();
    void h_blank();
    void v_blank();

public: //state machine
    PPU(Bus& bus, MMU& mmu, InterruptController& interrupt_controller);
    
    void connect_display(LCD* display) {
        screen = display;
    }

    using StateFunction = void (PPU::*)();  //pointer to state function
    enum class State {
        OAM_SCAN, PIXEL_TRANSFER, H_BLANK, V_BLANK,
    };
    StateFunction current_state;
    State curr_state_enum;
    //PPU is clocked in t-states (4 t-state = 1 m-cycle)
    void tick();

    void print_state();
};  

#endif