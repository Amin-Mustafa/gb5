#ifndef SPACES_H
#define SPACES_H

#include <cstdint>

namespace Space {
    enum TileMap : uint16_t {
        TILEMAP_0 = 0x9800,
        TILEMAP_1 = 0x9C00,
    };

    enum TileBlock: uint16_t {
        TILEBLOCK_0 = 0x8000,
        TILEBLOCK_1 = 0x8800,
        TILEBLOCK_2 = 0x9000,
    };

    enum ROM : uint16_t {
        ROM_START = 0x0000, ROM_END = 0x7FFF,
        ROM_BANK1_START = 0x0000, ROM_BANK1_END = 0x3FFF,
        ROM_BANK2_START = 0x4000, ROM_BANK2_END = 0x7FFF
    };

    enum RAM : uint16_t {
        EXTRAM_START = 0xA000, EXTRAM_END = 0xBFFF,
        WRAM_START = 0xC000, WRAM_END = 0xDFFF,
        ECHO_RAM_START = 0xE000, ECHO_RAM_END = 0xFDFF,
    };

    enum PPU_Register: uint16_t {
        LCDC = 0xFF40, STAT, SCY, SCX, LY, LYC, DMA, BGP, OBP0, OBP1, WY, WX,
    };

    enum HRAM : uint16_t {
        HRAM_START = 0xFF80, HRAM_END = 0xFFFE,
    };

    enum OAM : uint16_t {
        OAM_START = 0xFE00, OAM_END = 0xFE9F,
        OAM_RESERVED_START = 0xFEA0, OAM_RESERVED_END = 0xFEFF,
    };

    enum VRAM : uint16_t {
        VRAM_START = 0x8000, VRAM_END = 0x9FFF,
    };

    enum WaveRAM : uint16_t {
        WAVE_RAM_START = 0xFF30, WAVE_RAM_END = 0xFF3F,
    };
}

#endif