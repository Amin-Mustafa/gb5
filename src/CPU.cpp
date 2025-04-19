#include "../include/CPU.h"
#include "../include/Instruction.h"
#include <iostream>

namespace SM83 {

Instruction::Opfn decode_inst(uint8_t opcode) {
    if((opcode >= 40 && opcode <= 0x7f) && opcode != 0x76) 
        return load8;
    else return nullptr;
}

void CPU::fetch_and_execute() {
}
}