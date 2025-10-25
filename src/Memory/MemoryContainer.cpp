#include <memory>
#include "../../include/Memory/MemoryContainer.h"
#include "../../include/Memory/MMU.h"
#include "../../include/Memory/MemoryRegion.h"

MemoryContainer::MemoryContainer(MMU& mmu, uint16_t start, uint16_t end)
    :data(end - start + 1),
     start_addr{start},
     end_addr{end},
     region{this, start, end} 
     {
        mmu.add_region(&region);
     }