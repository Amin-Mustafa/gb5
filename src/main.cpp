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

void print_flags(const SM83::FlagRegister& fr) {
    std::cout << fr.get_flag(SM83::Flag::ZERO)
              << fr.get_flag(SM83::Flag::NEGATIVE)
              << fr.get_flag(SM83::Flag::HALF_CARRY)
              << fr.get_flag(SM83::Flag::CARRY);
}

int main() {
    SM83::MMU mem;
    SM83::ROM rom(mem);
    SM83::MemoryContainer ram(0x8000, 0xFFFF, mem);
    SM83::CPU cpu(mem);
    SM83::Disassembler dis(mem);

    rom.load("ROM/09-op r,r.gb");
    while(true) {
        dis.disassemble_at(cpu.pc);
        cpu.print_state();
        cpu.tick();
    }
}