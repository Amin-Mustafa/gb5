#ifndef MMU_H
#define MMU_h

#include <vector>
#include <cstdint>
#include <functional>
#include <string>

class MemoryRegion;


class MMU {
private:   
    std::vector<MemoryRegion*> memory_map;
    std::array<MemoryRegion*, 0x10000> memory_lookup;
    MemoryRegion* region_of(uint16_t addr);
public:
    MMU();
    void add_region(MemoryRegion* region);
    uint8_t read(uint16_t addr);
    void write(uint16_t addr, uint8_t data);
};

#endif