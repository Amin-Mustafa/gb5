#ifndef MMU_H
#define MMU_h

#include <vector>
#include <stdint.h>
#include <string>
#include "ROM.h"

namespace SM83 {

struct MemoryMap {
    //structure to initialize MMU memory map
    MemoryMap()
        :romspace{new ROM{0x0000, 0x8000}} {}

    ~MemoryMap() {
        delete romspace;
    }

    ROM* romspace;
};

class MMU {
public:
    MMU(MemoryMap& map)
        :rom{*(map.romspace)} {}


    uint8_t& operator[](uint16_t addr) {
        return read_address(addr);
    }
    void load(const std::string& filename) {
        rom.load(filename);
    }
private:
    ROM& rom;
    uint8_t& read_address(uint16_t addr);
};

}

#endif