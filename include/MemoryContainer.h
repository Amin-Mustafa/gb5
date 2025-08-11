#ifndef MEMORYCONTAINER_H
#define MEMORYCONTAINER_H

#include <vector>
#include <cstdint>

class MMU;

class MemoryContainer {
//generic R/W memory container 
private:
    std::vector<uint8_t> data;
public:
    MemoryContainer(MMU& mmu, uint16_t start, uint16_t end);
};    

#endif