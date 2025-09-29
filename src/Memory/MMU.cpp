#include "../../include/Memory/MMU.h"
#include "../../include/Memory/MemoryRegion.h"
#include "../../include/Memory/DmaController.h"
#include "../../include/Memory/Spaces.h"
#include <stdexcept>

MMU::MMU()  
    :dmac{std::make_unique<DmaController>()} 
    {}

MMU::~MMU() = default;

bool addr_in_hram(uint16_t addr);
bool addr_in_vram(uint16_t addr);

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
    if(dmac->active() && !addr_in_hram(addr)) {
        //ignore reads to values not in HRAM
        return 0xFF;    
    }
    return region_of(addr)->read(addr);
}
void MMU::write(uint16_t addr, uint8_t val) {
    if(dmac->active() && !addr_in_hram(addr)){
        //ignore writes not to HRAM
        return;         
    }
    region_of(addr)->write(addr, val);
}
uint8_t MMU::dma_read(uint16_t addr) {
    return region_of(addr)->read(addr);
}
void MMU::dma_write(uint16_t addr, uint8_t val) {
    //bypass bus and transfer val to addr directly
    region_of(addr)->write(addr, val);
}  

void MMU::start_dma(uint8_t page) {
    //decide whether to start DMA from WRAM or echo RAM
    uint16_t start_addr = (page <= 0xDF) ? (page << 8) : ((page-0x20) << 8);
    //start DMA
    dmac->start(start_addr);
}

void MMU::tick() {
    if(dmac->active()) {
        uint16_t src = dmac->start_address() + dmac->offset();
        uint16_t dest = Space::OAM_START + dmac->offset();
        uint8_t val = dma_read(src);
        dma_write(dest, val);
        dmac->tick();
    }
}

bool addr_in_hram(uint16_t addr) {
    return (Space::HRAM_START <= addr) && (addr <= Space::HRAM_END);
}
bool addr_in_vram(uint16_t addr) {
    return (Space::VRAM_START <= addr) && (addr <= Space::VRAM_END);
}