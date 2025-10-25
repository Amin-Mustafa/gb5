#ifndef ROM_H
#define ROM_H

#include <vector>
#include <cstdint>
#include <string>
#include "MemoryRegion.h"

class MMU;

class ROM {
private:
    std::vector<uint8_t> data;
    MemoryRegion region;
public:
    static constexpr uint16_t START = 0x0000;
    static constexpr uint16_t END   = 0x7FFF;

    ROM(MMU& memory);
    void load(const std::string& filename); //load cartridge

    uint8_t ext_read(uint16_t addr) {
        return data[addr - START];
    }
    void ext_write(uint16_t addr, uint8_t val) {
        
    }
};


#endif