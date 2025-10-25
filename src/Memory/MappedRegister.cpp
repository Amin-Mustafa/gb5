#include "../../include/Memory/MappedRegister.h"
#include "../../include/Memory/MMU.h"

MappedRegister::MappedRegister(MMU& mmu, uint16_t addr)
    :region {this, addr, addr} 
    {
        mmu.add_region(&region);
    }
    