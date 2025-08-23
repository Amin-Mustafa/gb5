#ifndef PPU_H
#define PPU_H

#include <memory>
#include "VRAM.h"
#include "Tile.h"
#include "../../include/Memory/MappedRegister.h"

class MMU;

class PPU {
private:
    VRAM vram;
    int cycles; 
public:
    using StateFunction = void (PPU::*)();  //pointer to state function
    StateFunction current_state;
    StateFunction update_state();

    PPU(MMU&);

    //PPU is clocked in t-states (4 t-state = 1 m-cycle)
    void tick() {
        (this->*current_state)();
        update_state();
    }
};  

#endif