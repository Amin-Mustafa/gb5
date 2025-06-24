#include "../include/MMU.h"
#include "../include/ROM.h"
  
namespace SM83{

MMU::MMU(MemoryMap& map)
    :rom{*(map.romspace)} {
        //TODO: initialize hardware registers
    }

uint8_t& MMU::read_address(uint16_t addr) {
    if(addr < rom.size()) return rom[addr];
    else return rom[0]; //temporarily
}

}