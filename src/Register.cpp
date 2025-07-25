#include "../include/Register.h"
#include "../include/CPU.h"

uint8_t MemRegister::get() const {
    uint16_t addr =  ( static_cast<uint16_t>(hi.get()) << 8 ) | lo.get();
    return cpu.read_memory(addr);
}

void MemRegister::set(uint8_t val) {
    uint16_t addr =  ( static_cast<uint16_t>(hi.get()) << 8 ) | lo.get();
    cpu.write_memory(addr, val);
}

uint8_t Immediate8::get() const {
    cpu.pc++;  
    return cpu.read_memory(cpu.pc);
}

uint16_t Immediate16::get() const {
    uint8_t hi, lo;
    cpu.pc++;
    hi = cpu.read_memory(cpu.pc);
    cpu.pc++;
    lo = cpu.read_memory(cpu.pc);
    return (static_cast<uint16_t>(hi) << 8) | lo;
}