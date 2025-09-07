#include "../../include/Graphics/PPURegs.h" 
#include "../../include/Memory/MMU.h" 

PPURegs::PPURegs(MMU& mmu) 
//read/write via MMU
    :region{
        mmu,
        START, END,
        [this](uint16_t addr) {
            return data[addr - START];
        },
        [this](uint16_t addr, uint8_t val) {
            switch(addr) {
                case LY: 
                    break;
                case STAT: 
                    //keep lower 3 bytes of STAT as they are
                    data[STAT-START] = (val & (~0x07)) | (data[STAT-START] & (0x07));
                    break;
                default: 
                    data[addr-START] = val; 
                    break;
            } 
        }
    }
    {}



