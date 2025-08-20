#include "MappedRegister.h"
#include "MMU.h"

MappedRegister::MappedRegister(MMU& mmu, uint16_t addr)
    :region {
        mmu,
        addr, addr,
        [this](uint16_t) {return data;},
        [this](uint16_t addr, uint8_t val) {data = val;}
    } {}