#ifndef MAPPEDREGISTER_H
#define MAPPED_REGISTER_H

#include <cstdint>

class MMU;

class MappedRegister {
//generic memory-mapped hardware register
//essentially the same as a MemoryContainer, 
//but with only one value.
private:    
    uint8_t data;
public:
    MappedRegister(MMU& mmu, uint16_t addr);
    uint8_t get() {return data;}
    void set(uint8_t val) {data = val;}
};

#endif
