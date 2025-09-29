#ifndef SPACES_H
#define SPACES_H

#include <cstdint>

namespace Space {
    enum TileMap : uint16_t {
        TILEMAP_0 = 0x9800,
        TILEMAP_1 = 0x9C00,
    };

    enum TileBlock: uint16_t {
        BLOCK_0 = 0x8000,
        BLOCK_1 = 0x8800,
        BLOCK_2 = 0x9000
    };

    enum PPU_Register: uint16_t {
        LCDC = 0xFF40, STAT, SCY, SCX, LY, LYC, DMA, BGP, OBP0, OBP1, WY, WX,
    };

    enum HRAM : uint16_t {
        HRAM_START = 0xFF80, HRAM_END = 0xFFFE,
    };

    enum OAM : uint16_t {
        OAM_START = 0xFE00, OAM_END = 0xFE9F,
    };

    enum VRAM : uint16_t {
        VRAM_START = 0x8000, VRAM_END = 0x9FFF,
    };
}

#endif