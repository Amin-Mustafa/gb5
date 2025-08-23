#include "../../include/Graphics/VRAM.h"
#include "../../include/Memory/MMU.h"

VRAM::VRAM(MMU& mmu)
    :region {
        mmu,
        0x8000, 0x9FFF,
        [this](uint16_t addr)->uint8_t { 
            if(accessible) return read(addr);
            else return 0xFF;
        },
        [this](uint16_t addr, uint8_t val) { if(accessible) write(addr, val); }
    }
    {}

uint8_t VRAM::read(uint16_t addr){ 
    //PPU-facing; unrestricted VRAM read
    return data[addr-0x8000];
}
void VRAM::write(uint16_t addr, uint8_t val) {
    //PPU-facing; unrestricted VRAM write
    data[addr-0x8000] = val;
}

Tile VRAM::tile_at(uint16_t addr){
    return Tile(&data[addr-0x8000]);
}