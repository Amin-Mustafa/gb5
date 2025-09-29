#ifndef PPUREGS_H
#define PPUREGS_H

#include <cstdint>
#include <array>
#include "../../include/Memory/MemoryRegion.h"

class MMU;

class PPURegs {
private:
    MemoryRegion region;    //external memory mapping
    static constexpr uint16_t START = 0xFF40;
    static constexpr uint16_t END   = 0xFF4B;
public:
    PPURegs(MMU&);
    uint8_t lcdc, stat, scy, scx, ly, lyc, dma, bgp, obp_0, obp_1, wy, wx;

    uint8_t read(uint16_t addr) const;
    void write(MMU& mmu, uint16_t addr, uint8_t val);
};  

//LCDC bit checks
namespace LCDC {
    inline bool lcd_enable(const PPURegs& regs) {
        return (regs.lcdc >> 7) & (uint8_t)1;
    }
    inline bool win_tilemap(const PPURegs& regs) {
        return (regs.lcdc >> 6) & (uint8_t)1;
    }
    inline bool win_enable(const PPURegs& regs) {
        return (regs.lcdc >> 5) & (uint8_t)1;
    }
    inline bool bg_tile_area(const PPURegs& regs) {
        return (regs.lcdc >> 4) & (uint8_t)1;
    }
    inline bool bg_tilemap(const PPURegs& regs) {
        return (regs.lcdc >> 3) & (uint8_t)1;
    }
    inline bool obj_size(const PPURegs& regs) {
        return (regs.lcdc >> 2) & (uint8_t)1;
    }
    inline bool obj_enable(const PPURegs& regs) {
        return (regs.lcdc >> 1) & (uint8_t)1;
    }
    inline bool bgwin_enable(const PPURegs& regs) {
        return (regs.lcdc >> 0) & (uint8_t)1;
    }
}

//STAT stuff
namespace STAT {
    enum Source : uint8_t{
        MODE_0_SELECT = 3, MODE_1_SELECT, MODE_2_SELECT, LYC_INT_SELECT
    };
    enum Mode : uint8_t {
        MODE_0, MODE_1, MODE_2, MODE_3
    };
    constexpr uint8_t LYC_FLAG_BIT = 2;

    inline void update_lyc_flag(PPURegs& regs) {
        regs.stat = (regs.stat & ~((uint8_t)1 << LYC_FLAG_BIT)) | 
        ((uint8_t)(regs.ly == regs.lyc) << LYC_FLAG_BIT);
    }
    inline bool source_enabled(const PPURegs& regs, STAT::Source cond) {
        return (regs.stat >> cond) & (uint8_t)1;
    }     
    inline void set_mode(PPURegs& regs, STAT::Mode mode) {
        regs.stat = (regs.stat & ~0x03) | mode;
    }
    bool stat_line(const PPURegs& regs);
} //STAT

#endif