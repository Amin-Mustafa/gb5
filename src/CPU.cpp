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
    switch(opcode) {
        case 0x00: return Instruction{nop, {}, 1, 4, "NOP"};
        case 0x01: return Instruction{load8, {BC.pair, AF.hi}, 3,12, "LD BC,n16"};
        case 0x02: return Instruction{load8, {memory[BC.pair], AF.hi}, 1,8, "LD [BC],A"};
        case 0x03: return Instruction{inc16, {ref(BC.pair)}, 1,8, "INC BC"};
        case 0x04: return Instruction{inc8, {ref(BC.hi), ref(AF.lo)}, 1,8, "INC B"};
    }
}

void CPU::fetch_and_execute() {
    Instruction inst = decode(memory[pc]);
    inst.print();
    inst.execute(*this);
    pc += inst.length();
}
}