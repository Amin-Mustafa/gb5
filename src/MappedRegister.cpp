#include "MappedRegister.h"
#include "MemoryRegion.h"
#include "MMU.h"

MappedRegister::MappedRegister(MMU& mmu, uint16_t addr) {
    mmu.add_region(
        std::make_unique<MemoryRegion> (
            addr, addr,
            [this](uint16_t) {return data;},
            [this](uint16_t addr, uint8_t val) {data = val;}
        )
    );
}