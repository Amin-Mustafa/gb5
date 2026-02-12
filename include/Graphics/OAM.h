#ifndef OAM_H
#define OAM_H

#include <array>
#include <cstdint>
#include "Sprite.h"
#include "Memory/MMU.h" 

class MMU;

class OAM {
private:
    std::array<uint8_t, 0xA0> container;
public:
    static constexpr uint16_t START = 0xFE00;
    static constexpr uint16_t END   = 0xFE9F;

    OAM(MMU& mmu) {
        mmu.map_region(START, END, container.data());
    } 
    uint8_t read(uint16_t addr) const {
        return container[addr-START];
    }
    void write(uint16_t addr, uint8_t val) {
        container[addr-START] = val;
    }
    Sprite sprite_at(uint16_t addr) {
        return Sprite{&container[addr - START]};
    }
};  

#endif