#include "../include/MemoryContainer.h"
#include "../include/MMU.h"

MemoryContainer::MemoryContainer(uint16_t start, uint16_t end, MMU& mmu)
    :data(end - start + 1), 
    region{
        mmu,
        start, end,
        [this, start](uint16_t addr)->uint8_t {return data[addr - start];},
        [this, start](uint16_t addr, uint8_t val) {data[addr-start] = val;}
    } {}