#include <memory>
#include "../include/MemoryContainer.h"
#include "../include/MMU.h"
#include "../include/MemoryRegion.h"

MemoryContainer::MemoryContainer(MMU& mmu, uint16_t start, uint16_t end)
    :data(end - start + 1)
    {
        mmu.add_region(
            std::make_unique<MemoryRegion> (
                start, end,
                [this, start](uint16_t addr)->uint8_t {return data[addr - start];},
                [this, start](uint16_t addr, uint8_t val) {data[addr-start] = val;}
            )
        ); 
    }