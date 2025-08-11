#include "../include/MMU.h"
#include "../include/MemoryRegion.h"
#include <stdexcept>

MMU::MMU() = default;
MMU::~MMU() = default;

void MMU::add_region(std::unique_ptr<MemoryRegion> region){
    for(int addr = region->start(); addr <= region->end(); ++addr){
        memory_lookup[addr] = region.get();
    }
    memory_map.push_back(std::move(region));    //transfer ownership to memory_map
}
MemoryRegion* MMU::region_of(uint16_t addr) {
    MemoryRegion* region = memory_lookup[addr];
    if(region) 
        return region;
    
    throw std::runtime_error("Reference to unmapped memory region!");
}
uint8_t MMU::read(uint16_t addr) {
    return region_of(addr)->read(addr);
}
void MMU::write(uint16_t addr, uint8_t val) {
    region_of(addr)->write(addr, val);
}