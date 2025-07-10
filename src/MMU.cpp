#include "../include/MMU.h"

namespace SM83{
void MMU::add_region(MemoryRegion* region){
    memory_map.push_back(region);
}


}