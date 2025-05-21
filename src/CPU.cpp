#include "../include/CPU.h"
#include "../include/Instruction.h"
#include "../include/MMU.h"
#include <iostream>
#include <format>

namespace SM83 {

constexpr uint16_t pair(uint8_t hi, uint8_t lo) {
    return (hi << 8) | lo;
}

Instruction CPU::decode(uint8_t opcode) {
    using std::ref;
    uint8_t& byte1 = memory[pc+1];
    uint8_t& byte2 = memory[pc+2];
    switch(opcode) {
        using namespace Operation;
        case 0x00: return Instruction{[](){}, 1, 4};    //NOP
        case 0x01: return LD_16(B,C, byte2,byte1);
        case 0x02: return LD_8_mem(memory[pair(B,C)], A);
        case 0x03: return INC_16(B,C);
        case 0x04: return INC_8(B,F);
        case 0x05: return DEC_8(B,F);
        case 0x06: return LD_8_imm(B, byte1);
        case 0x07: return Instruction {
            [this](){
                F = flag_state(0, 0, 0, A >> 7);
                A = (A << 1) | (A >> 7);
            },
            1, 4
        };
        case 0x08: return Instruction {
            [this,byte2,byte1](){
                memory[pair(byte2, byte1)] = sp & 0xff;
                memory[pair(byte2, byte1)+1] = sp >> 8;
            },
            3,20
        };
        case 0x09: return ADD_16(H,L,B,C,F);
        case 0x0A: return LD_8_mem(A, memory[pair(B,C)]);
        case 0x0B: return DEC_16(B,C);
        case 0x0C: return INC_8(C, F);
        case 0x0D: return DEC_8(C, F);
        case 0x0E: return LD_8_imm(C, byte1);
        case 0x0F: return Instruction {
            [this](){
                F = flag_state(0, 0, 0, A & 1);
                A = (A >> 1) | (A << 7);
            },
            1, 4
        };
        default: return Instruction{[](){}, 1, 4};
    }
}

void CPU::fetch_and_execute() {
    Instruction inst = decode(memory[pc]);
    inst.execute();
    pc += inst.length();
}

void CPU::push_to_stack(uint16_t num) {
    memory[sp-1] = num >> 8;
	memory[sp-2] = num & 0xff;
	sp -= 2;
}

void CPU::pop_from_stack(uint8_t& num_hi, uint8_t& num_lo) {
    num_lo = memory[sp];
    num_hi = memory[sp+1];
    sp += 2;
}
void CPU::pop_from_stack(uint16_t& num) {
    num = pair(memory[sp+1], memory[sp]);
    sp += 2;
}

//internal CPU operations
Instruction CPU::JR(uint8_t offset, bool condition) {
    if(condition) {
        return Instruction{[this, offset](){ pc += (int8_t)offset; }, 2, 12};
    }
    else return Instruction{[](){}, 2,8};
}
Instruction CPU::JP(uint16_t destination, bool condition) {
    if(condition) {
        return Instruction{[this, destination](){ pc = destination; }, 3, 16};
    }
    return Instruction{[](){}, 3, 12};
}
Instruction CPU::CALL(uint16_t destination, bool condition) {
    if(condition) {
        return Instruction{
            [this, destination]() {
                push_to_stack(pc);
                pc = destination;
            }, 
            3, 24
        };
    }
    else return Instruction{[](){}, 3, 12};
}
Instruction CPU::PUSH(uint8_t num_hi, uint8_t num_lo) {
    return Instruction {
        [this, num_hi, num_lo](){
         push_to_stack(pair(num_hi, num_lo)); 
        },
        1, 16
    };
}
Instruction CPU::POP(uint8_t& num_hi, uint8_t& num_lo) {
    return Instruction {
        [this, &num_hi, &num_lo](){
            pop_from_stack(num_hi, num_lo);
        },
        1, 12
    };
}
Instruction CPU::RET(bool condition) {
    if(condition) {
        return Instruction{[this](){pop_from_stack(pc);}, 1, 20};
    }
    else return Instruction{[](){}, 1, 8};
}
Instruction CPU::RST(uint8_t destination) {
    return Instruction{[this, destination](){pc = destination;}, 1, 16};
}
}