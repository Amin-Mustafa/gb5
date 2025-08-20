#include "../include/MemoryRegion.h"
#include "../include/MMU.h"

MemoryRegion::MemoryRegion(MMU& mmu, uint16_t start, uint16_t end, ReadFunc read_func, WriteFunc write_func)
    :start_addr{start}, end_addr{end},
     read{std::move(read_func)}, write{std::move(write_func)}
    {
        mmu.add_region(this);
    }