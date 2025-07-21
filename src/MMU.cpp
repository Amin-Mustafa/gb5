#include "../include/MMU.h"
#include "../include/MemoryRegion.h"

MMU::MMU() {
    std::fill(memory_lookup.begin(), memory_lookup.end(), nullptr);
}

void MMU::add_region(MemoryRegion* region){
    memory_map.push_back(region);
    for(int addr = region->start(); addr <= region->end(); ++addr){
        memory_lookup[addr] = region;
    }
}
MemoryRegion* MMU::region_of(uint16_t addr) {
    return memory_lookup[addr];
}
uint8_t MMU::read(uint16_t addr) {
    return region_of(addr)->read(addr);
}
void MMU::write(uint16_t addr, uint8_t val) {
    region_of(addr)->write(addr, val);
}