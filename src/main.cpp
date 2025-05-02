#include "../include/Instruction.h"
#include "../include/CPU.h"
#include "../include/MMU.h"
#include <iostream>

uint16_t pair(uint8_t x, uint8_t y) {
    return x << 8 | y;
}

int main() {
    SM83::MemoryMap map;
    SM83::MMU mem(map);
    SM83::CPU cpu(mem);

    cpu.pc = 0x100;
    cpu.memory[cpu.pc] = 0x04;
    cpu.memory[cpu.pc + 1] = 0x11;
    cpu.memory[cpu.pc + 2] = 0x91;
    cpu.BC.hi = 0xff;
    cpu.BC.lo = 0x21;
    cpu.AF.hi = 0x44;
    cpu.AF.lo = 0x00;
    cpu.HL.hi = 0x21;
    cpu.HL.lo = 0x12;
    cpu.memory[cpu.HL.pair] = 0xA5;
    printf("BEFORE INC: B=%02x, F=%02x, pc=%04x\n", cpu.BC.hi, cpu.AF.lo, cpu.pc);
    cpu.tick();
    printf("BEFORE INC: B=%02x, F=%02x, pc=%04x\n", cpu.BC.hi, cpu.AF.lo, cpu.pc);
}