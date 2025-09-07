#ifndef PPU_H
#define PPU_H

#include <memory>
#include "VRAM.h"
#include "OAM.h"
#include "PPURegs.h"
#include "../../include/Memory/MemoryRegion.h"

class MMU;
class InterruptController;

class PPU {
private:
    VRAM vram;
    OAM oam;

    //hardware registers
    enum Register {
        LCDC, STAT, SCY, SCX, LY, LYC, DMA, BGP, OBP0, OBP1, WY, WX
    };
    PPURegs regs;
    
    int cycles; 
    InterruptController& ic;
public:
    using StateFunction = void (PPU::*)();  //pointer to state function
    StateFunction current_state;

    PPU(MMU& mmu, InterruptController& interrupt_controller);

    //PPU is clocked in t-states (4 t-state = 1 m-cycle)
    void tick() {(this->*current_state)();}

    //PPU states
    void oam_scan();
    void drawing();
    void h_blank();
    void v_blank();
};  

#endif