#ifndef MMU_H
#define MMU_h

#include <vector>
#include <stdint.h>
#include <functional>
#include <string>
#include "ROM.h"
#include "MemoryMap.h"

namespace SM83 {

class MMU {
public:
    MMU(MemoryMap& map);

    uint8_t& operator[](uint16_t addr) {
        return read_address(addr);
    }

    void load(const std::string& filename) {rom.load(filename);}
private:
    ROM& rom;
    uint8_t& read_address(uint16_t addr);
};

}

#endif