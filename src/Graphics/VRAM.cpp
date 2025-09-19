#include "../../include/Graphics/VRAM.h"
#include "../../include/Memory/MMU.h"
#include "../../include/Graphics/Spaces.h"
#include "../../include/Graphics/Tile.h"

VRAM::VRAM(MMU& mmu)
    :region {
        mmu,
        START, END,
        [this](uint16_t addr)->uint8_t { 
            if(accessible) return read(addr);
            else return 0xFF;
        },
        [this](uint16_t addr, uint8_t val) { if(accessible) write(addr, val); }
    }
    {
        write(Space::BLOCK_0 + 0x10, 0xFF);
        write(Space::BLOCK_0 + 0x11, 0x00);
        write(Space::BLOCK_0 + 0x12, 0x7E);
        write(Space::BLOCK_0 + 0x13, 0xFF);
        write(Space::BLOCK_0 + 0x14, 0x85);
        write(Space::BLOCK_0 + 0x15, 0x81);
        write(Space::BLOCK_0 + 0x16, 0x89);
        write(Space::BLOCK_0 + 0x17, 0x83);
        write(Space::BLOCK_0 + 0x18, 0x93);
        write(Space::BLOCK_0 + 0x19, 0x85);
        write(Space::BLOCK_0 + 0x1A, 0xA5);
        write(Space::BLOCK_0 + 0x1B, 0x8B);
        write(Space::BLOCK_0 + 0x1C, 0xC9);
        write(Space::BLOCK_0 + 0x1D, 0x97);
        write(Space::BLOCK_0 + 0x1E, 0x7E);
        write(Space::BLOCK_0 + 0x1F, 0xFF);

        write(Space::TILEMAP_0, 0x01);
    }

uint8_t VRAM::read(uint16_t addr) const { 
    //PPU-facing; unrestricted VRAM read
    return data[addr-0x8000];
}
void VRAM::write(uint16_t addr, uint8_t val) {
    //PPU-facing; unrestricted VRAM write
    data[addr-0x8000] = val;
}

Tile VRAM::tile_at(uint8_t index, AddressMode addr_mode) const {
    uint8_t offset = index * 0x10;
    switch(addr_mode) {
        case AddressMode::UNSIGNED:
            return Tile{&data[Space::BLOCK_0 + offset - START]};
        case AddressMode::SIGNED:
            return Tile{&data[Space::BLOCK_2 + static_cast<int8_t>(offset) - START]};
        default:    
            return Tile();
    }
}