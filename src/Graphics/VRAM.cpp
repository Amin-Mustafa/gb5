#include "../../include/Graphics/VRAM.h"
#include "../../include/Memory/MMU.h"
#include "../../include/Memory/MemoryRegion.h"

VRAM::VRAM(MMU& mmu)
    :region{
        mmu,
        0x8000, 0x9FFF,
        [this](uint16_t addr){return read(addr);},
        [this](uint16_t addr, uint8_t val){return write(addr, val);}
    } {}

uint8_t VRAM::read(uint16_t addr) {
    constexpr uint16_t map1_start = 0x9800;
    constexpr uint16_t map2_start = 0x9C00;
    constexpr uint16_t map2_end = 0x9FFF;

    if(addr >= block0.start() && addr <= block1.start()) {
        return block0.read(addr);
    }
    if(addr >= block1.start() && addr <= block2.start()) {
        return block1.read(addr);
    }
    if(addr >= block2.start() && addr <= map1_start) {
        return block2.read(addr);
    }
    if(addr >= map1_start && addr < map2_start) {
        return map1[addr - map1_start];
    }
    if(addr >= map2_start && addr <= map2_end) {
        return map2[addr - map2_start];
    }
}

void VRAM::write(uint16_t addr, uint8_t val) {
    
}