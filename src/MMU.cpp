#include "../include/MMU.h"
#include "../include/ROM.h"

namespace SM83{

enum MEMORY_MAP{
    ROM_START = 0x0000
};

uint8_t& MMU::_read_address(uint16_t addr) {
    if(addr >= ROM_START) return _rom[addr];
}
}