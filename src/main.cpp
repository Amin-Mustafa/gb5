#include "../include/MMU.h"
#include "../include/ROM.h"
#include "../include/MemoryContainer.h"
#include "../include/Register.h"

#include "../include/CPU.h"
#include "../include/Disassembler.h"
#include <iostream>

uint16_t pair(uint8_t hi, uint8_t lo) {
    return hi << 8 | lo;
}

void print_flags(const FlagRegister& fr) {
    std::cout << fr.get_flag(Flag::ZERO)
              << fr.get_flag(Flag::NEGATIVE)
              << fr.get_flag(Flag::HALF_CARRY)
              << fr.get_flag(Flag::CARRY);
}

int main() {
    MMU mem;
    ROM rom(mem);
    MemoryContainer ram(0x8000, 0xFFFF, mem);
    CPU cpu(mem);
    Disassembler dis(mem);

    rom.load("ROM/test.gb");
    while(true) {
        dis.disassemble_at(cpu.pc);
        cpu.print_state();
        cpu.tick();
        std::cin.get();
    }
}