#include "../include/MemoryContainer.h"
#include "../include/MMU.h"

MemoryContainer::MemoryContainer(MMU& mmu, uint16_t start, uint16_t end)
    :data(end - start + 1), 
    region{
        mmu,
        start, end,
        [this, start](uint16_t addr)->uint8_t {return data[addr - start];},
        [this, start](uint16_t addr, uint8_t val) {data[addr-start] = val;}
    } {}

MappedRegister::MappedRegister(MMU& mmu, uint16_t addr)
    :region {
        mmu,
        addr, addr,
        [this](uint16_t) {return reg;},
        [this](uint16_t addr, uint8_t val) {reg = val;}
    } {}