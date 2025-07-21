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
    void load(const std::string& filename); //load cartridge
    ROM(MMU& mmu);
};


#endif