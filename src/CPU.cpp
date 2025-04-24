#include "../include/CPU.h"
#include "../include/Instruction.h"
#include "../include/MMU.h"
#include <iostream>

namespace SM83 {

Instruction CPU::decode(uint8_t opcode) { 
    switch(opcode) {
        case 0x78:
            return Instruction{load8, {std::ref(A), std::ref(B)}, 4, "LD A B"};
        case 0xFB:
            return Instruction{EI, 4, "EI"};
        default: break;
    }
}

void CPU::fetch_and_execute() {
    Instruction inst = decode(memory[pc++]);
    inst.print();
    inst.execute(*this);
}
}