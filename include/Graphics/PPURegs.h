#ifndef PPUREGS_H
#define PPUREGS_H

#include <cstdint>
#include <array>
#include "../../include/Memory/MemoryRegion.h"

class MMU;

class PPURegs {
private:
    std::array<uint8_t, 12> data;
    MemoryRegion region;    //external memory mapping
    static constexpr uint16_t START = 0xFF40;
    static constexpr uint16_t END   = 0xFF4B;
public:
    PPURegs(MMU&);

    enum Register {
        LCDC = START, STAT, SCY, SCX, LY, LYC, DMA, BGP, OBP0, OBP1, WY, WX,
    };

    enum STAT_Condition {
        MODE_0 = 3, MODE_1, MODE_2, LYC_INT_SELECT
    };

    //internal read/write via enum
    uint8_t read(Register reg) {return data[reg - START];}
    void write(Register reg, uint8_t val) {data[reg - START] = val;}

    bool stat_source_on(STAT_Condition cond) {
        //check if bit corresponding to cond is set in STAT
        return (data[STAT - START] >> cond) & (uint8_t)1;
    };
};  

#endif