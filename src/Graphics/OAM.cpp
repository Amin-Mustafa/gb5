#include "../../include/Graphics/OAM.h"
#include "../../include/Memory/MMU.h"

OAM::OAM(MMU& mmu) 
    :region{
        mmu,
        START, END,
        [this](uint16_t addr) {return read(addr);},
        [this](uint16_t addr, uint8_t val) {return write(addr, val);}
    }
    {}