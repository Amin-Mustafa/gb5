#include "../include/Register.h"
#include "../include/CPU.h"

uint8_t MemRegister::get() const {
    uint16_t addr =  ( static_cast<uint16_t>(hi) << 8 ) | lo;
    return cpu.read_memory(addr);
}

void MemRegister::set(uint8_t val) {
    uint16_t addr =  ( static_cast<uint16_t>(hi) << 8 ) | lo;
    cpu.write_memory(addr, val);
}