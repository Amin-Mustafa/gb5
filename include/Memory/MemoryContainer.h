#ifndef MEMORYCONTAINER_H
#define MEMORYCONTAINER_H

#include <vector>
#include <cstdint>
#include "MemoryRegion.h"

class MMU;

class MemoryContainer {
//generic R/W memory container 
private:
    std::vector<uint8_t> data;
    uint16_t start_addr;
    uint16_t end_addr;
    MemoryRegion region;
public:
    MemoryContainer(MMU& mmu, uint16_t start, uint16_t end);

    uint8_t ext_read(uint16_t addr) {
        return data[addr - start_addr];
    }
    void ext_write(uint16_t addr, uint8_t val) {
        data[addr - start_addr] = val;
    }
};    



#endif