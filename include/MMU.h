#ifndef MMU_H
#define MMU_h

#include <vector>
#include <stdint.h>
#include <ROM.h>

namespace SM83 {

struct MemoryMap {
    //structure to initialize MMU memory map
    MemoryMap()
        :_romspace{new ROM{0x0000, 0x8000}} {}

    ~MemoryMap() {
        delete _romspace;
    }

    ROM* _romspace;
};

class MMU {
public:
    MMU(MemoryMap& map)
        :_rom{*(map._romspace)} {}


    uint8_t& operator[](uint16_t addr) {
        return _read_address(addr);
    }
private:
    ROM& _rom;
    uint8_t& _read_address(uint16_t addr);
};

}

#endif