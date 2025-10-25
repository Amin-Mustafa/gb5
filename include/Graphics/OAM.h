#ifndef OAM_H
#define OAM_H

#include <array>
#include <cstdint>
#include "../../include/Memory/MemoryRegion.h"
#include "Sprite.h"

class MMU;

class OAM {
private:
    std::array<uint8_t, 0xA0> data;
    MemoryRegion region;
public:
    static constexpr uint16_t START = 0xFE00;
    static constexpr uint16_t END   = 0xFE9F;

    OAM(MMU&); 
    bool accessible = true;
    uint8_t read(uint16_t addr) const {return data[addr-START];}
    void write(uint16_t addr, uint8_t val) {data[addr-START] = val;}
    Sprite sprite_at(uint16_t addr) {return Sprite{&data[addr - START]};}

    uint8_t ext_read(uint16_t addr) {
        return accessible ? data[addr - START] : 0xFF;
    } 
    void ext_write(uint16_t addr, uint8_t val) {
        if(!accessible) return;
        data[addr - START] = val;
    }
};  

#endif