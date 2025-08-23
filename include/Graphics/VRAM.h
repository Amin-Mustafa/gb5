#ifndef VRAM_H
#define VRAM_H

#include <cstdint>
#include "../../include/Memory/MemoryRegion.h"
#include "Tile.h"

class MMU;

class VRAM {
private:
    std::array<uint8_t, 0x2000> data;
    MemoryRegion region;

public:
    VRAM(MMU& mmu);
    uint8_t read(uint16_t addr);
    void write(uint16_t addr, uint8_t val);
    Tile tile_at(uint16_t addr);

    bool accessible;    //whether VRAM is accessible via MMU

    //unsigned (0x8000) address mode vs signed (0x8800) address mode
    enum class AddressMode{UNSIGNED, SIGNED};

    static constexpr uint16_t TILEDATA_1 = 0x8000;
    static constexpr uint16_t TILEDATA_2 = 0x8800;
    static constexpr uint16_t TILEDATA_3 = 0x9000;
    static constexpr uint16_t TILEMAP_1 = 0x9800;
    static constexpr uint16_t TILEMAP_2 = 0x9C00;
};

#endif