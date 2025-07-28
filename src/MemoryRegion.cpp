#include "../include/MemoryRegion.h"
#include "../include/MMU.h"

MemoryRegion::MemoryRegion(MMU& mem, uint16_t start, uint16_t end, ReadFunc read_func, WriteFunc write_func)
    :start_addr{start}, end_addr{end}, data(end - start + 1) 
    {
        read = std::move(read_func);
        write = std::move(write_func);
        mem.add_region(this);
    }