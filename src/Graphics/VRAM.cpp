#include "../../include/Graphics/VRAM.h"
#include "../../include/Memory/MMU.h"
#include "../../include/Memory/Spaces.h"
#include "../../include/Graphics/Tile.h"
#include "../../include/Graphics/PPU.h"
#include <format>

VRAM::VRAM(MMU& mmu)
    :region {this, START, END}
    {
        mmu.add_region(&region);
        //test tile
       /*write(Space::BLOCK_0 + 0x00, 0xFF);
        write(Space::BLOCK_0 + 0x01, 0x00);
        write(Space::BLOCK_0 + 0x02, 0x7E);
        write(Space::BLOCK_0 + 0x03, 0xFF);
        write(Space::BLOCK_0 + 0x04, 0x85);
        write(Space::BLOCK_0 + 0x05, 0x81);
        write(Space::BLOCK_0 + 0x06, 0x89);
        write(Space::BLOCK_0 + 0x07, 0x83);
        write(Space::BLOCK_0 + 0x08, 0x93);
        write(Space::BLOCK_0 + 0x09, 0x85);
        write(Space::BLOCK_0 + 0x0A, 0xA5);
        write(Space::BLOCK_0 + 0x0B, 0x8B);
        write(Space::BLOCK_0 + 0x0C, 0xC9);
        write(Space::BLOCK_0 + 0x0D, 0x97);
        write(Space::BLOCK_0 + 0x0E, 0x7E);
        write(Space::BLOCK_0 + 0x0F, 0xFF);

        write(Space::BLOCK_0 + 0x10, 0xFF);
        write(Space::BLOCK_0 + 0x11, 0xFF);
        write(Space::BLOCK_0 + 0x12, 0x81);
        write(Space::BLOCK_0 + 0x13, 0x81);
        write(Space::BLOCK_0 + 0x14, 0x81);
        write(Space::BLOCK_0 + 0x15, 0x81);
        write(Space::BLOCK_0 + 0x16, 0x81);
        write(Space::BLOCK_0 + 0x17, 0x81);
        write(Space::BLOCK_0 + 0x18, 0x81);
        write(Space::BLOCK_0 + 0x19, 0x81);
        write(Space::BLOCK_0 + 0x1A, 0x81);
        write(Space::BLOCK_0 + 0x1B, 0x81);
        write(Space::BLOCK_0 + 0x1C, 0x81);
        write(Space::BLOCK_0 + 0x1D, 0x81);
        write(Space::BLOCK_0 + 0x1E, 0xFF);
        write(Space::BLOCK_0 + 0x1F, 0xFF);

        write(Space::BLOCK_0 + 0x20, 0x00);
        write(Space::BLOCK_0 + 0x21, 0xFF);
        write(Space::BLOCK_0 + 0x22, 0x00);
        write(Space::BLOCK_0 + 0x23, 0xFF);
        write(Space::BLOCK_0 + 0x24, 0x00);
        write(Space::BLOCK_0 + 0x25, 0xFF);
        write(Space::BLOCK_0 + 0x26, 0x00);
        write(Space::BLOCK_0 + 0x27, 0xFF);
        write(Space::BLOCK_0 + 0x28, 0x00);
        write(Space::BLOCK_0 + 0x29, 0xFF);
        write(Space::BLOCK_0 + 0x2A, 0x00);
        write(Space::BLOCK_0 + 0x2B, 0xFF);
        write(Space::BLOCK_0 + 0x2C, 0x00);
        write(Space::BLOCK_0 + 0x2D, 0xFF);
        write(Space::BLOCK_0 + 0x2E, 0x00);
        write(Space::BLOCK_0 + 0x2F, 0xFF);

        write(Space::TILEMAP_0 + 0x00, 0x00); */
    }

uint8_t VRAM::read(uint16_t addr) const { 
    //PPU-facing; unrestricted VRAM read
    return data[addr - START];
}
void VRAM::write(uint16_t addr, uint8_t val) {
    //PPU-facing; unrestricted VRAM write
    data[addr - START] = val;
}

Tile VRAM::tile_at(uint8_t index, AddressMode addr_mode) const {
    uint16_t addr = START;
    switch(addr_mode) {
        case AddressMode::UNSIGNED:
            addr = Space::BLOCK_0 + index*0x10;
            break;
        case AddressMode::SIGNED:
            addr = Space::BLOCK_2 + static_cast<int8_t>(index)*0x10;
            break;
    }
    return Tile{ &data[addr - START] };
}