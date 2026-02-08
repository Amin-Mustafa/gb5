#ifndef MMU_H
#define MMU_H

#include <vector>
#include <cstdint>
#include <memory>
#include <array>

class MemoryRegion;
class DmaController;
class Bus;

class MMU {
private:   
    std::array<MemoryRegion*, 0x10000> memory_lookup;
    MemoryRegion* region_of(uint16_t addr);
public:
    MMU(Bus& bus);
    ~MMU();
    void add_region(MemoryRegion* region);
    uint8_t read(uint16_t addr);
    void write(uint16_t addr, uint8_t data);
};

#endif