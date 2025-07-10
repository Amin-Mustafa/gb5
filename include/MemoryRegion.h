#ifndef MEMORYREGION_H
#define MEMORYREGION_H

#include <functional>
#include <cstdint>

namespace SM83{
    
class MemoryRegion {
private:
    typedef std::function<uint8_t(uint16_t)> ReadFunc;
    typedef std::function<void(uint16_t, uint8_t)> WriteFunc;

    std::vector<uint8_t> data;
    uint16_t start_addr;
    uint16_t end_addr;

public:
    ReadFunc read;
    WriteFunc write;
    uint16_t start() const {return start_addr;}
    uint16_t end() const {return end_addr;}

    MemoryRegion(uint16_t start, uint16_t end, ReadFunc read_func, WriteFunc write_func)
        :start_addr{start}, end_addr{end}, data(end_addr-start_addr + 1) {
            read = std::move(read_func);
            write = std::move(write_func);
        }
};

}
#endif