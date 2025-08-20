#ifndef MMU_H
#define MMU_h

#include <vector>
#include <cstdint>
#include <memory>
#include <array>

class MemoryRegion;

class MMU {
private:   
    MemoryRegion* memory_lookup[0x10000] = {};
    MemoryRegion* region_of(uint16_t addr);
public:
    MMU();
    ~MMU();
    void add_region(MemoryRegion* region);
    uint8_t read(uint16_t addr);
    void write(uint16_t addr, uint8_t data);
};

#endif