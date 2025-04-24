#ifndef ROM_H
#define ROM_H

#include <vector>
#include <stdint.h>

namespace SM83 {

class ROM {
private:
    uint16_t start_addr;
    std::vector<uint8_t> contents;
public:
    ROM(uint16_t start, uint16_t size)
        :start_addr{start}, contents(size) {}

    bool contains(uint16_t addr) const {
        return addr > start_addr && addr < contents.size(); 
    }

    uint8_t& operator[] (uint16_t addr) {
        return contents[addr - start_addr];
    }
    uint16_t start() {return start_addr;}
};

}

#endif