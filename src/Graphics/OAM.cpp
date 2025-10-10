#include "../../include/Graphics/OAM.h"
#include "../../include/Memory/MMU.h"

OAM::OAM(MMU& mmu) 
    :region{
        mmu,
        START, END,
        [this](uint16_t addr)->uint8_t{
            if(!accessible) return 0xFF;
            return read(addr);
        },
        [this](uint16_t addr, uint8_t val) {
            if(!accessible) return;
            write(addr, val);
        }
    }
    {
        //test sprites
        write(START + 0x00, 16);
        write(START + 0x01, 24);
        write(START + 0x02, 1); 

        write(START + 0x10, 16);
        write(START + 0x11, 33);
        write(START + 0x12, 1);
    }