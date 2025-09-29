#include "../../include/Graphics/OAM.h"
#include "../../include/Memory/MMU.h"

OAM::OAM(MMU& mmu) 
    :region{
        mmu,
        START, END,
        [this](uint16_t addr)->uint8_t{
            return read(addr);
        },
        [this](uint16_t addr, uint8_t val) {
            write(addr, val);
        }
    }
    {}