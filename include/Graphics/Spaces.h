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

    enum Register: uint16_t {
        LCDC = 0xFF40, STAT, SCY, SCX, LY, LYC, DMA, BGP, OBP0, OBP1, WY, WX,
    };
}
#endif