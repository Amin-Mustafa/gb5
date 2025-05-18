#include "../include/CPU.h"
#include "../include/MMU.h"
#include "../include/Register.h"
#include <iostream>

uint16_t pair(uint8_t hi, uint8_t lo) {
    return hi << 8 | lo;
}

void print_flags(const SM83::FlagRegister& fr) {
    std::cout << fr.get_flag(SM83::Flag::ZERO)
              << fr.get_flag(SM83::Flag::NEGATIVE)
              << fr.get_flag(SM83::Flag::HALF_CARRY)
              << fr.get_flag(SM83::Flag::CARRY);
}

int main() {
    SM83::MemoryMap map;
    SM83::MMU mem(map);
    SM83::CPU cpu(mem);

    cpu.pc = 0x100;
    cpu.memory[cpu.pc] = 0x06;
    cpu.memory[cpu.pc + 1] = 0x31;
    cpu.memory[cpu.pc + 2] = 0x54;
    cpu.B = 0x05;
    cpu.C = 0x34;
    cpu.A = 0x0f;

    printf("BEFORE TICK: A = %02x, B = %02x, F = ", cpu.A, cpu.B);
    print_flags(cpu.F);
    printf(", PC = %04x\n", cpu.pc);
    cpu.tick();
    printf("BEFORE TICK: A = %02x, B = %02x, F = ", cpu.A, cpu.B);
    print_flags(cpu.F);
    printf(", PC = %04x\n", cpu.pc);
}