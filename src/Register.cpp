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

uint8_t HighMemory::get() const {
    cpu.pc++;
    uint8_t offset = cpu.read_memory(cpu.pc);   //4 cycles
    return cpu.read_memory(0xFF00 + offset);    //4 cycles
    //total: 8 cycles
}

void HighMemory::set(uint8_t val) {
    cpu.pc++;
    uint8_t offset = cpu.read_memory(cpu.pc);
    cpu.write_memory(0xFF00 + offset, val);
}

uint16_t Immediate16::get() const {
    uint8_t hi, lo;
    cpu.pc++;
    lo = cpu.read_memory(cpu.pc);
    cpu.pc++;
    hi = cpu.read_memory(cpu.pc);
    return (static_cast<uint16_t>(hi) << 8) | lo;
}