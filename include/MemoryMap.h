#ifndef MEMORY_MAP_H
#define MEMORY_MAP_H

#include <array>
#include <functional>

struct MemoryMap {
private:
    static std::array<uint8_t, 0xFFFF> central_memory;
    struct MemoryRegion {
        //A memory region is a designated zone in the memory
        //with specific read and write behavior
        uint16_t start_addr, end_addr;
        std::function<uint8_t(uint16_t)> read_func;
        std::function<void(uint16_t)> write_func;
    };
    //the memory map is a list of regions
    std::vector<MemoryRegion> region_map;   
public:
    //structure to initialize memory map
    MemoryMap();
};

#endif