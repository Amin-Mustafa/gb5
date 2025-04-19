#ifndef MMU_H
#define MMU_h

#include <vector>
#include <stdint.h>

namespace SM83 {
    
class ROM;

class MMU {
public:
    MMU(ROM& rom)
        :_rom{rom} {}

    uint8_t& operator[](uint16_t addr) {
        return _read_address(addr);
    }
private:
    ROM& _rom;
    uint8_t& _read_address(uint16_t addr);
};

}

#endif