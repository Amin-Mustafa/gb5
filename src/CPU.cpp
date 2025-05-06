#include "../include/CPU.h"
#include "../include/Instruction.h"
#include "../include/MMU.h"
#include <iostream>

namespace SM83 {

constexpr uint16_t pair(uint8_t hi, uint8_t lo) {
    return (hi << 8) | lo;
}

Instruction CPU::decode(uint8_t opcode) {
    using std::ref;
    uint8_t& byte1 = memory[pc+1];
    uint8_t& byte2 = memory[pc+2];
    printf("%04x\n", pc);
    switch(opcode) {
        case 0x00: return Instruction{nop, {}, 1, 4, "NOP"};
        case 0x02: return Instruction{load8, {memory[pair(B,C)], A}, 2,8, "LD [BC],A"};
    }
}

void CPU::fetch_and_execute() {
    Instruction inst = decode(memory[pc]);
    inst.print();
    inst.execute();
    pc += inst.length();
}
}