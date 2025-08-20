#include <memory>
#include "../include/Memory/MemoryContainer.h"
#include "../include/Memory/MMU.h"
#include "../include/Memory/MemoryRegion.h"

MemoryContainer::MemoryContainer(MMU& mmu, uint16_t start, uint16_t end)
    :data(end - start + 1),
    region{
        mmu,
        start, end,
        [this, start](uint16_t addr)->uint8_t {return data[addr - start];},
        [this, start](uint16_t addr, uint8_t val) {data[addr-start] = val;}
    } {}