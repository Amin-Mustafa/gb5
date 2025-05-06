#include "../include/CPU.h"
#include "../include/MMU.h"
#include <iostream>

uint16_t pair(uint8_t hi, uint8_t lo) {
    return hi << 8 | lo;
}

int main() {
    SM83::MemoryMap map;
    SM83::MMU mem(map);
    SM83::CPU cpu(mem);

    cpu.pc = 0x100;
    cpu.memory[cpu.pc] = 0x01;
    cpu.memory[cpu.pc + 1] = 0x31;
    cpu.memory[cpu.pc + 2] = 0x54;
    cpu.B = 0x12;
    cpu.C = 0x34;

    printf("BEFORE TICK: BC = %02x%02x, PC = %04x\n", cpu.B, cpu.C, cpu.pc);
    cpu.tick();
    printf("AFTER TICK: BC = %02x%02x, PC = %04x\n", cpu.B, cpu.C, cpu.pc);
}