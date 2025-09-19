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
    void write(uint16_t addr, uint8_t val);

    enum STATCondition {
        MODE_0_SELECT = 3, MODE_1_SELECT, MODE_2_SELECT, LYC_INT_SELECT
    };
    bool stat_source_on(STATCondition cond) const {
        return (stat >> cond) & (uint8_t)1;
    }
    bool stat_line() {
        uint8_t mode = stat & 0x03;    
        return (
            (stat_source_on(MODE_0_SELECT) && (mode == 0))    ||
            (stat_source_on(MODE_1_SELECT) && (mode == 1))    ||
            (stat_source_on(MODE_2_SELECT) && (mode == 2))    ||
            (stat_source_on(LYC_INT_SELECT)&& (lyc == ly)) 
        );
    }
};  

//LCDC bit checks
inline bool lcdc_lcd_enable(const PPURegs& regs) {
    return (regs.lcdc >> 7) & (uint8_t)1;
}
inline bool lcdc_win_tilemap(const PPURegs& regs) {
    return (regs.lcdc >> 6) & (uint8_t)1;
}
inline bool lcdc_win_enable(const PPURegs& regs) {
    return (regs.lcdc >> 5) & (uint8_t)1;
}
inline bool lcdc_bg_tile_area(const PPURegs& regs) {
    return (regs.lcdc >> 4) & (uint8_t)1;
}
inline bool lcdc_bg_tilemap(const PPURegs& regs) {
    return (regs.lcdc >> 3) & (uint8_t)1;
}
inline bool lcdc_obj_size(const PPURegs& regs) {
    return (regs.lcdc >> 2) & (uint8_t)1;
}
inline bool lcdc_obj_enable(const PPURegs& regs) {
    return (regs.lcdc >> 1) & (uint8_t)1;
}
inline bool lcdc_bgwin_enable(const PPURegs& regs) {
    return (regs.lcdc >> 0) & (uint8_t)1;
}

#endif