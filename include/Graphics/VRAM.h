#ifndef VRAM_H
#define VRAM_H

#include <cstdint>
#include <array>
#include "memory/MMU.h"
#include "Tile.h"

class Tile;
class PPU;

class VRAM {
public:
    static constexpr uint16_t START = 0x8000;
    static constexpr uint16_t END = 0x9FFF;
    enum class AddressMode {UNSIGNED, SIGNED};

    VRAM(MMU& mmu)
        {
            mmu.map_region(START, END, data.data());
        }

    uint8_t read(uint16_t addr) const { 
        //PPU-facing; unrestricted VRAM read
        return data[addr - START];
    }
    void write(uint16_t addr, uint8_t val) {
        //PPU-facing; unrestricted VRAM write
        data[addr - START] = val;
    }

    Tile tile_at(uint8_t index, AddressMode addr_mode) const {
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

    void block(MMU& mmu) {
        if(accessible) {
            accessible = false;
            mmu.unmap_region(START, END);
        }
    }
    
    void unblock(MMU& mmu) {
        if(!accessible) {
            mmu.map_region(START, END, data.data());
            accessible = true;
        }
    }

private:
    std::array<uint8_t, 0x2000> data;
    bool accessible = true;
};

#endif