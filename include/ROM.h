#ifndef ROM_H
#define ROM_H

#include <vector>
#include <stdint.h>

namespace SM83 {

class ROM {
private:
    uint16_t _start;
    std::vector<uint8_t> _contents;
public:
    ROM(uint16_t start_address, uint16_t size)
        :_start{start_address}, _contents(size) {}

    bool contains(uint16_t addr) const {
        return addr > _start && addr < _contents.size(); 
    }

    uint8_t& operator[] (uint16_t addr) {
        return _contents[addr - _start];
    }
};

}

#endif