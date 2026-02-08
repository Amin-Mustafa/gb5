#include "Memory/MMU.h"
#include "Memory/Bus.h"
#include "Memory/MemoryRegion.h"
#include "Memory/DmaController.h"
#include "Memory/Spaces.h"
#include <stdexcept>

MMU::MMU(Bus& bus)  
    {
        bus.connect(*this);
    }

MMU::~MMU() = default;

void MMU::add_region(MemoryRegion* region){
    printf("MAPPED REGION FROM %04x to %04x\n", region->start(), region->end());
    for(int addr = region->start(); addr <= region->end(); ++addr){
        memory_lookup[addr] = region;
    }
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