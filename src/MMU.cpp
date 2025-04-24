#include "../include/MMU.h"
#include "../include/ROM.h"

namespace SM83{


uint8_t& MMU::read_address(uint16_t addr) {
    if(addr >= rom.start()) return rom[addr];
}
}