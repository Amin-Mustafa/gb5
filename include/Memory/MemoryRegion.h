#ifndef MEMORYREGION_H
#define MEMORYREGION_H

#include <functional>
#include <cstdint>

class MMU;

class MemoryRegion {
private:
    uint16_t start_addr;
    uint16_t end_addr;

    typedef std::function<uint8_t(uint16_t)> ReadFunc;
    typedef std::function<void(uint16_t, uint8_t)> WriteFunc;
public:
    ReadFunc read;
    WriteFunc write;
    uint16_t start() const {return start_addr;}
    uint16_t end() const {return end_addr;}

    MemoryRegion(MMU& mmu, uint16_t start, uint16_t end, ReadFunc read_func, WriteFunc write_func);
};

#endif