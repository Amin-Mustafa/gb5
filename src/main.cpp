#include "../include/CPU.h"
#include "../include/MMU.h"
#include "../include/Register.h"
#include "../include/Disassembler.h"
#include "../include/Instruction.h"
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
    SM83::Disassembler dis(mem);

    mem.load("ROM/09-op r,r.gb");
    while(true) {
        dis.disassemble_at(cpu.pc);
        cpu.print_state();
        cpu.tick();
    }
}