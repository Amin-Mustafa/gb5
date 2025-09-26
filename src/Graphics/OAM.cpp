#include "../../include/Graphics/OAM.h"
#include "../../include/Memory/MMU.h"

OAM::OAM(MMU& mmu) 
    :region{
        mmu,
        START, END,
        [this](uint16_t addr)->uint8_t{
            if(accessible) return read(addr);
            else return 0xFF;
        },
        [this](uint16_t addr, uint8_t val) {
            if(accessible) write(addr, val);
        }
    }
    {}