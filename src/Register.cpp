#include "../include/Register.h"
#include "../include/MMU.h"

uint8_t MemRegister::get() const {
    uint16_t addr =  ( static_cast<uint16_t>(hi) << 8 ) | lo;
    return mem.read(addr);
}

void MemRegister::set(uint8_t val) {
    uint16_t addr =  ( static_cast<uint16_t>(hi) << 8 ) | lo;
    mem.write(addr, val);
}