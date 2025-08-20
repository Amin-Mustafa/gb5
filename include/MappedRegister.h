#ifndef MAPPEDREGISTER_H
#define MAPPEDREGISTER_H

#include <cstdint>
#include "MemoryRegion.h"

class MMU;

class MappedRegister {
//generic memory-mapped hardware register
//essentially the same as a MemoryContainer, 
//but with only one value.
private:    
    uint8_t data;
    MemoryRegion region;
public:
    MappedRegister(MMU& mmu, uint16_t addr);
    uint8_t get() {return data;}
    void set(uint8_t val) {data = val;}
};

#endif
