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
    {}

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