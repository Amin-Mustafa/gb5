#ifndef VRAM_H
#define VRAM_H

#include <cstdint>
#include "../../include/Memory/MemoryRegion.h"
#include "Tile.h"

class MMU;

class VRAM {
private:
    //3 tile data blocks at 0x8000, 0x8800, and 0x9000
    TileBlock block0{0x8000};
    TileBlock block1{0x8800};
    TileBlock block2{0x9000};
    //two maps of 32x32 tile indices
    std::array<uint8_t, 0x400> map1; 
    std::array<uint8_t, 0x400> map2; 

    MemoryRegion region;
public:
    VRAM(MMU& mmu);
    uint8_t read(uint16_t addr);
    void write(uint16_t addr, uint8_t val);
};

#endif