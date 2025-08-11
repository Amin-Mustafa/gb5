#ifndef MEMORYCONTAINER_H
#define MEMORYCONTAINER_H

#include "MemoryRegion.h"

class MMU;

class MemoryContainer {
//generic R/W memory container 
private:
    std::vector<uint8_t> data;
    MemoryRegion region;
public:
    MemoryContainer(MMU& mmu, uint16_t start, uint16_t end);
};    

class MappedRegister {
//generic memory-mapped hardware register
//essentially the same as a MemoryContainer, 
//but with only one value.
private:    
    uint8_t reg;
    MemoryRegion region;
public:
    MappedRegister(MMU& mmu, uint16_t addr);
    uint8_t get() {return reg;}
    void set(uint8_t val) {reg = val;}
};

#endif