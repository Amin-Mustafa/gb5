#ifndef VRAM_H
#define VRAM_H

#include <cstdint>
#include "../../include/Memory/MemoryRegion.h"

class MMU;
class Tile;
class PPU;

class VRAM {
public:
    static constexpr uint16_t START = 0x8000;
    static constexpr uint16_t END = 0x9FFF;

    VRAM(MMU& mmu);

    bool accessible{true};

    uint8_t read(uint16_t addr) const ;
    void write(uint16_t addr, uint8_t val);

    uint8_t ext_read(uint16_t addr) {
        return accessible ? data[addr - START] : 0xFF;
    } 
    void ext_write(uint16_t addr, uint8_t val) {
        if(!accessible) return;
        data[addr - START] = val;
    }

    enum class AddressMode {UNSIGNED, SIGNED};

    Tile tile_at(uint8_t index, AddressMode mode) const;

private:
    std::array<uint8_t, 0x2000> data;
    MemoryRegion region;
};

#endif