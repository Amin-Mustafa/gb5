#include "../../include/Graphics/PPURegs.h" 
#include "../../include/Memory/MMU.h" 
#include "../../include/Graphics/Spaces.h"


PPURegs::PPURegs(MMU& mmu) 
//read/write via MMU
    :region{
        mmu,
        START, END,
        [this](uint16_t addr) { return read(addr); },
        [this](uint16_t addr, uint8_t val) { return write(addr, val); }
    }
    {
        //defaults
        lcdc    = 0x91;
        stat    = 0x85;
        scy     = 0x00;
        scx     = 0x00;
        ly      = 0x00;
        lyc     = 0x00;
        dma     = 0xFF;
        bgp     = 0xFC;
        obp_0   = 0x00;
        obp_1   = 0x00;
        wy      = 0x00;
        wx      = 0x00;

    }

uint8_t PPURegs::read(uint16_t addr) const {
    switch(addr) {
        case Space::LCDC: return lcdc;
        case Space::STAT: return stat;
        case Space::SCY : return scy;
        case Space::SCX : return scx;
        case Space::LY  : return ly;
        case Space::LYC : return lyc;
        case Space::DMA : return dma;
        case Space::BGP : return bgp;
        case Space::OBP0: return obp_0;
        case Space::OBP1: return obp_1;
        case Space::WY  : return wy;
        case Space::WX  : return wx;
        default: return 0xFF;
    }
}
void PPURegs::write(uint16_t addr, uint8_t val) {
    switch(addr) {
        case Space::LCDC: lcdc  = val;  break;
        case Space::STAT: stat = ((val & ~0x03) | (stat & 0x03)); break;
        case Space::SCY : scy   = val;  break;
        case Space::SCX : scx   = val;  break;
        case Space::LY  : break; //read only
        case Space::LYC : lyc   = val;  break;
        case Space::DMA : dma   = val;  break;
        case Space::BGP : bgp   = val;  break;
        case Space::OBP0: obp_0 = val;  break;
        case Space::OBP1: obp_1 = val;  break;
        case Space::WY  : wy    = val;  break;
        case Space::WX  : wx    = val;  break;
        default: break;
    }
}

namespace STAT {
    bool stat_line(const PPURegs& regs) {
        uint8_t mode = regs.stat & 0x03;    
        return (
            (source_enabled(regs, MODE_0_SELECT)  && (mode == 0))    ||
            (source_enabled(regs, MODE_1_SELECT)  && (mode == 1))    ||
            (source_enabled(regs, MODE_2_SELECT)  && (mode == 2))    ||
            (source_enabled(regs, LYC_INT_SELECT) && (regs.lyc == regs.ly)) 
        );
    }
}



