#ifndef VRAM_H
#define VRAM_H

#include <cstdint>
#include "../../include/Memory/MemoryRegion.h"

class MMU;
class Tile;

enum class TileMap : uint16_t {
    TILEMAP_0 = 0x9800,
    TILEMAP_1 = 0x9C00
};

class VRAM {
public:
    VRAM(MMU& mmu);

    bool accessible;    //whether VRAM is externally accessible via MMU
    uint8_t read(uint16_t addr) const ;
    void write(uint16_t addr, uint8_t val);

    enum class AddressMode {UNSIGNED, SIGNED};

    Tile tile_at(uint8_t index, AddressMode mode) const;

private:
    std::array<uint8_t, 0x2000> data;
    MemoryRegion region;
    static constexpr uint16_t START = 0x8000;
    static constexpr uint16_t END = 0x9FFF;
};

#endif