#include <iostream>
#include <format>
#include "../include/CPU.h"
#include "../include/Instruction.h"
#include "../include/MMU.h"
#include "../include/Disassembler.h"

namespace SM83 {

constexpr uint16_t pair(uint8_t hi, uint8_t lo) {
    return (hi << 8) | lo;
}
constexpr void inc_pair(uint8_t& hi, uint8_t& lo){
    uint16_t pair = (hi << 8) | lo;
    pair++;
    hi = pair >> 8;
    lo = pair & 0xff;
}
constexpr void dec_pair(uint8_t& hi, uint8_t& lo){
    uint16_t pair = (hi << 8) | lo;
    pair--;
    hi = pair >> 8;
    lo = pair & 0xff;
}

void CPU::fetch_and_execute() {
    Instruction inst = decode(memory[pc]);
    inst.execute();
    cycles += inst.cycles();
    pc += inst.length();
}

Instruction CPU::decode(uint8_t opcode) {
    using std::ref;
    #define N8 memory[pc+1]
    #define N16 pair(memory[pc+2], memory[pc+1])
    switch(opcode) {
        using namespace Operation;
        //0x00 - 0x0F
        case 0x00: return Instruction{[](){}, 1, 4};    //NOP
        case 0x01: return LD_16(B,C, N16);
        case 0x02: return LD_8_mem(memory[pair(B,C)], A);
        case 0x03: return INC_16(B,C);
        case 0x04: return INC_8(B,F);
        case 0x05: return DEC_8(B,F);
        case 0x06: return LD_8_imm(B, N8);
        case 0x07: return Instruction {
            [this](){
                F = flag_state(0, 0, 0, A >> 7);
                A = (A << 1) | (A >> 7);
            },
            1, 4
        };
        case 0x08: return Instruction {
            [this](){
                memory[N16] = sp & 0xff;
                memory[N16 + 1] = sp >> 8;
            },
            3,20
        };
        case 0x09: return ADD_16(H,L, pair(B,C) ,F);
        case 0x0A: return LD_8_mem(A, memory[pair(B,C)]);
        case 0x0B: return DEC_16(B,C);
        case 0x0C: return INC_8(C, F);
        case 0x0D: return DEC_8(C, F);
        case 0x0E: return LD_8_imm(C, N8);
        case 0x0F: return Instruction {
            [this](){
                F = flag_state(0, 0, 0, A & 1);
                A = (A >> 1) | (A << 7);
            },
            1, 4
        };

        //0x10 - 0x1F
        case 0x10: break;   //TODO
        case 0x11: return LD_16(D,E,N16);
        case 0x12: return LD_8_mem(memory[pair(D,E)], A);
        case 0x13: return INC_16(D,E);
        case 0x14: return INC_8(D, F);
        case 0x15: return DEC_8(D, F);
        case 0x16: return LD_8_imm(D, N8);
        case 0x17: return Instruction{
            [this]() {
                bool old_carry = F.get_flag(Flag::CARRY);
                F = flag_state(0, 0, 0, A >> 7);
                A = ((A << 1) & ~((uint8_t)1)) | old_carry;
            }, 
            1,4
        };
        case 0x18: return JR(N8, true);
        case 0x19: return ADD_16(H,L, pair(D,E) ,F);
        case 0x1A: return LD_8_mem(A, memory[pair(D,E)]);
        case 0x1B: return DEC_16(D,E);
        case 0x1C: return INC_8(E,F);
        case 0x1D: return DEC_8(E,F);
        case 0x1E: return LD_8_imm(E,N8);
        case 0x1F: return Instruction{
            [this](){
                uint8_t old_carry = F.get_flag(Flag::CARRY);
                F = flag_state(0, 0, 0, A & 1);
                A = ((A >> 1) & ~((uint8_t)1 << 7)) | (old_carry << 7);
            },
            1,4
        };
        
        //0x20 - 0x2F
        case 0x20: return JR(N8, !F.get_flag(Flag::ZERO));
        case 0x21: return LD_16(H,L, N16);
        case 0x22: return Instruction {
            [this]() {
                memory[pair(H,L)] = A;
                inc_pair(H, L);
            },
            1,8
        };
        case 0x23: return INC_16(H,L);
        case 0x24: return INC_8(H,F);
        case 0x25: return DEC_8(H,F);
        case 0x26: return LD_8_imm(H,N8);
        case 0x27: break;   //TODO: DAA
        case 0x28: return JR(N8, F.get_flag(Flag::ZERO));
        case 0x29: return ADD_16(H,L, pair(H,L), F);
        case 0x2A: return Instruction {
            [this]() {
                A = memory[pair(H,L)];
                inc_pair(H,L);
            },
            1,8
        };
        case 0x2B: return DEC_16(H,L);
        case 0x2C: return INC_8(L,F);
        case 0x2D: return DEC_8(L,F);
        case 0x2E: return LD_8_imm(L,N8);
        case 0x2F: return Instruction {
            [this](){
                A = ~A;
                F.set_flag(Flag::NEGATIVE, 1);
                F.set_flag(Flag::HALF_CARRY, 1);
            },
            1,4
        };
        
        //0x30 - 0x3F
        case 0x30: return JR(N8, !F.get_flag(Flag::CARRY));
        case 0x31: return Instruction{
            [this]() {sp = N16;}, 3,12  //doesn't change any flags
        };
        case 0x32: return Instruction {
            [this]() {
                memory[pair(H,L)] = A;
                dec_pair(H, L);
            },
            1,8
        };
        case 0x33: return Instruction{[this](){sp++;}, 1,8};
        case 0x34: return INC_8_mem(memory[pair(H,L)], F);
        case 0x35: return DEC_8_mem(memory[pair(H,L)], F);
        case 0x36: return Instruction{
            [this]() { memory[pair(H,L)] = memory[pc + 1]; }, 2,12
        };
        case 0x37: return Instruction {
            [this]() { 
                F = flag_state(F.get_flag(Flag::ZERO), 0, 0, 1);
            },
            1, 4 
        };
        case 0x38: return JR(N8, F.get_flag(Flag::CARRY));
        case 0x39: return ADD_16(H,L, sp, F);
        case 0x3A: return Instruction {
            [this]() {
                A = memory[pair(H,L)];
                dec_pair(H,L);
            },
            1,8
        };
        case 0x3B: return Instruction{[this](){sp--;}, 1,8};
        case 0x3C: return INC_8(A,F);
        case 0x3D: return DEC_8(A,F);
        case 0x3E: return LD_8_imm(A,N8);
        case 0x3F: return Instruction {
            [this]() { 
                F = flag_state(F.get_flag(Flag::ZERO), 0, 0, !F.get_flag(Flag::CARRY));
            },
            1, 4 
        };

        //0x40 - 0x4F
        case 0x40: return LD_8_reg(B,B);
        case 0x41: return LD_8_reg(B,C);
        case 0x42: return LD_8_reg(B,D);
        case 0x43: return LD_8_reg(B,E);
        case 0x44: return LD_8_reg(B,H);
        case 0x45: return LD_8_reg(B,L);
        case 0x46: return LD_8_mem(B,memory[pair(H,L)]);
        case 0x47: return LD_8_reg(B,A);
        case 0x48: return LD_8_reg(C,B);
        case 0x49: return LD_8_reg(C,C);
        case 0x4A: return LD_8_reg(C,D);
        case 0x4B: return LD_8_reg(C,E);
        case 0x4C: return LD_8_reg(C,H);
        case 0x4D: return LD_8_reg(C,L);
        case 0x4E: return LD_8_mem(C,memory[pair(H,L)]);
        case 0x4F: return LD_8_reg(C,A);

        //0x50 - 0x5F
        case 0x50: return LD_8_reg(D,B);
        case 0x51: return LD_8_reg(D,C);
        case 0x52: return LD_8_reg(D,D);
        case 0x53: return LD_8_reg(D,E);
        case 0x54: return LD_8_reg(D,H);
        case 0x55: return LD_8_reg(D,L);
        case 0x56: return LD_8_mem(D,memory[pair(H,L)]);
        case 0x57: return LD_8_reg(D,A);
        case 0x58: return LD_8_reg(E,B);
        case 0x59: return LD_8_reg(E,C);
        case 0x5A: return LD_8_reg(E,D);
        case 0x5B: return LD_8_reg(E,E);
        case 0x5C: return LD_8_reg(E,H);
        case 0x5D: return LD_8_reg(E,L);
        case 0x5E: return LD_8_mem(E,memory[pair(H,L)]);
        case 0x5F: return LD_8_reg(E,A);

        //0x60 - 0x6F
        case 0x60: return LD_8_reg(H,B);
        case 0x61: return LD_8_reg(H,C);
        case 0x62: return LD_8_reg(H,D);
        case 0x63: return LD_8_reg(H,E);
        case 0x64: return LD_8_reg(H,H);
        case 0x65: return LD_8_reg(H,L);
        case 0x66: return LD_8_mem(H,memory[pair(H,L)]);
        case 0x67: return LD_8_reg(H,A);
        case 0x68: return LD_8_reg(L,B);
        case 0x69: return LD_8_reg(L,C);
        case 0x6A: return LD_8_reg(L,D);
        case 0x6B: return LD_8_reg(L,E);
        case 0x6C: return LD_8_reg(L,H);
        case 0x6D: return LD_8_reg(L,L);
        case 0x6E: return LD_8_mem(L,memory[pair(H,L)]);
        case 0x6F: return LD_8_reg(L,A);

        //0x70 - 0x7F
        case 0x70: return LD_8_mem(memory[pair(H,L)],B);
        case 0x71: return LD_8_mem(memory[pair(H,L)],C);
        case 0x72: return LD_8_mem(memory[pair(H,L)],D);
        case 0x73: return LD_8_mem(memory[pair(H,L)],E);
        case 0x74: return LD_8_mem(memory[pair(H,L)],H);
        case 0x75: return LD_8_mem(memory[pair(H,L)],L);
        case 0x76: break;   //TODO: HALT
        case 0x77: return LD_8_mem(memory[pair(H,L)],A);
        case 0x78: return LD_8_reg(A,B);
        case 0x79: return LD_8_reg(A,C);
        case 0x7A: return LD_8_reg(A,D);
        case 0x7B: return LD_8_reg(A,E);
        case 0x7C: return LD_8_reg(A,H);
        case 0x7D: return LD_8_reg(A,L);
        case 0x7E: return LD_8_mem(A,memory[pair(H,L)]);
        case 0x7F: return LD_8_reg(A,A);

        //0x80 - 0x8F
        case 0x80: return ADD_8_reg(A,B,0,F);
        case 0x81: return ADD_8_reg(A,C,0,F);
        case 0x82: return ADD_8_reg(A,D,0,F);
        case 0x83: return ADD_8_reg(A,E,0,F);
        case 0x84: return ADD_8_reg(A,H,0,F);
        case 0x85: return ADD_8_reg(A,L,0,F);
        case 0x86: return ADD_8_mem(A,memory[pair(H,L)],0,F);
        case 0x87: return ADD_8_reg(A,A,0,F);
        case 0x88: return ADD_8_reg(A,B,1,F);
        case 0x89: return ADD_8_reg(A,C,1,F);
        case 0x8A: return ADD_8_reg(A,D,1,F);
        case 0x8B: return ADD_8_reg(A,E,1,F);
        case 0x8C: return ADD_8_reg(A,H,1,F);
        case 0x8D: return ADD_8_reg(A,L,1,F);
        case 0x8E: return ADD_8_mem(A,memory[pair(H,L)],1,F);
        case 0x8F: return ADD_8_reg(A,A,1,F);

        //0x90 - 0x9F
        case 0x90: return SUB_8_reg(A,B,0,F);
        case 0x91: return SUB_8_reg(A,C,0,F);
        case 0x92: return SUB_8_reg(A,D,0,F);
        case 0x93: return SUB_8_reg(A,E,0,F);
        case 0x94: return SUB_8_reg(A,H,0,F);
        case 0x95: return SUB_8_reg(A,L,0,F);
        case 0x96: return SUB_8_mem(A,memory[pair(H,L)],0,F);
        case 0x97: return SUB_8_reg(A,A,0,F);
        case 0x98: return SUB_8_reg(A,B,1,F);
        case 0x99: return SUB_8_reg(A,C,1,F);
        case 0x9A: return SUB_8_reg(A,D,1,F);
        case 0x9B: return SUB_8_reg(A,E,1,F);
        case 0x9C: return SUB_8_reg(A,H,1,F);
        case 0x9D: return SUB_8_reg(A,L,1,F);
        case 0x9E: return SUB_8_mem(A,memory[pair(H,L)],1,F);
        case 0x9F: return SUB_8_reg(A,A,1,F);

        //0xA0 - 0xAF
        case 0xA0: return AND_8_reg(A,B,F);
        case 0xA1: return AND_8_reg(A,C,F);
        case 0xA2: return AND_8_reg(A,D,F);
        case 0xA3: return AND_8_reg(A,E,F);
        case 0xA4: return AND_8_reg(A,H,F);
        case 0xA5: return AND_8_reg(A,L,F);
        case 0xA6: return AND_8_mem(A,memory[pair(H,L)],F);
        case 0xA7: return AND_8_reg(A,A,F);
        case 0xA8: return XOR_8_reg(A,B,F);
        case 0xA9: return XOR_8_reg(A,C,F);
        case 0xAA: return XOR_8_reg(A,D,F);
        case 0xAB: return XOR_8_reg(A,E,F);
        case 0xAC: return XOR_8_reg(A,H,F);
        case 0xAD: return XOR_8_reg(A,L,F);
        case 0xAE: return XOR_8_mem(A,memory[pair(H,L)],F);
        case 0xAF: return XOR_8_reg(A,A,F);
        
        //0xB0 - 0xBF
        case 0xB0: return OR_8_reg(A,B,F);
        case 0xB1: return OR_8_reg(A,C,F);
        case 0xB2: return OR_8_reg(A,D,F);
        case 0xB3: return OR_8_reg(A,E,F);
        case 0xB4: return OR_8_reg(A,H,F);
        case 0xB5: return OR_8_reg(A,L,F);
        case 0xB6: return OR_8_mem(A,memory[pair(H,L)],F);
        case 0xB7: return OR_8_reg(A,A,F);
        case 0xB8: return CP_8_reg(A,B,F);
        case 0xB9: return CP_8_reg(A,C,F);
        case 0xBA: return CP_8_reg(A,D,F);
        case 0xBB: return CP_8_reg(A,E,F);
        case 0xBC: return CP_8_reg(A,H,F);
        case 0xBD: return CP_8_reg(A,L,F);
        case 0xBE: return CP_8_mem(A,memory[pair(H,L)],F);
        case 0xBF: return CP_8_reg(A,A,F);

        //0xC0 - 0xCF
        case 0xC0: return RET(!F.get_flag(Flag::ZERO));
        case 0xC1: return POP(B, C);
        case 0xC2: return JP(N16, !F.get_flag(Flag::ZERO));
        case 0xC3: return JP(N16, true);
        case 0xC4: return CALL(N16, !F.get_flag(Flag::ZERO));
        case 0xC5: return PUSH(B,C);
        case 0xC6: return ADD_8_imm(A, N8, 0, F);
        case 0xC7: return RST(0x00);
        case 0xC8: return RET(F.get_flag(Flag::ZERO));
        case 0xC9: return RET(true);
        case 0xCA: return JP(N16, F.get_flag(Flag::ZERO));
        case 0xCB: break;   //TODO: PREFIX
        case 0xCC: return CALL(N16, F.get_flag(Flag::ZERO));
        case 0xCD: return CALL(N16, true);
        case 0xCE: return ADD_8_imm(A, N8, 1, F);
        case 0xCF: return RST(0x08);
        
        //0xD0 - 0xDF
        case 0xD0: return RET(!F.get_flag(Flag::CARRY));
        case 0xD1: return POP(D,E);
        case 0xD2: return JP(N16, !F.get_flag(Flag::CARRY));
        case 0xD3: break;   //NOP
        case 0xD4: return CALL(N16, !F.get_flag(Flag::CARRY));
        case 0xD5: return PUSH(D,E);
        case 0xD6: return SUB_8_imm(A, N8, 0, F);
        case 0xD7: return RST(0x10);
        case 0xD8: return Instruction {
            [this](){pop_from_stack(pc);}, 1, 16
        };
        case 0xD9: return Instruction{
            [this](){
                pop_from_stack(pc);
                int_enable = true;
                current_state = fetch_and_execute;
            }, 1, 16
        };
        case 0xDA: return JP(N16, F.get_flag(Flag::CARRY));
        case 0xDB: break;   //NOP
        case 0xDC: return JP(N16, F.get_flag(Flag::CARRY));
        case 0xDD: break;   //NOP
        case 0xDE: return SUB_8_imm(A, N8, 1, F);
        case 0xDF: return RST(0x18);
        
        //0xE0 - 0xEF
        case 0xE0: return Instruction{[this](){memory[0xFF00 + N8] = A;}, 2, 12};
        case 0xE1: return POP(H,L);
        case 0xE2: return Instruction{[this](){memory[0xFF00 + C]  = A;}, 1, 8};
        case 0xE3: break;   //NOP
        case 0xE4: break;   //NOP
        case 0xE5: return PUSH(H,L);
        case 0xE6: return AND_8_imm(A, N8, F);
        case 0xE7: return RST(0x20);
        case 0xE8: return ADD_16_e8(sp, N8, F);
        case 0xE9: return Instruction{
            [this](){pc = pair(H,L);}, 1, 4
        };
        case 0xEA: return Instruction{
            [this](){memory[N16] = A;}, 3, 16
        };
        case 0xEB:
        case 0xEC:
        case 0xED: break;   //NOP
        case 0xEE: return XOR_8_imm(A, N8, F);
        case 0xEF: return RST(0x28);

        //0xF0 - 0xFF
        case 0xF0: return Instruction{[this](){A = memory[0xFF00 + N8];}, 2,12};
        case 0xF1: return Instruction{
            [this](){
                uint8_t flags = F.get_state();
                pop_from_stack(A, flags);
                F = flags;
            },
            1, 12
        };
        case 0xF2: return Instruction{[this](){A = memory[0xFF00  + C];}, 1, 8};
        case 0xF3: return DI();
        case 0xF4: break;   //NOP
        case 0xF5: PUSH(A, F.get_state());
        case 0xF6: OR_8_imm(A, N8, F);
        case 0xF7: RST(0x30);
        case 0xF8: return Instruction {
            [this](){
                uint16_t sp_val = sp;
                ADD_16_e8(sp_val, N8, F).execute();
                H = sp_val >> 8;
                L = sp_val & 0xff;
            },
            2,12
        };
        case 0xF9: return Instruction{[this](){sp = pair(H,L);}, 1,8};
        case 0xFA: return Instruction{[this](){A = memory[N16];}, 3,16};
        case 0xFB: return EI();
        case 0xFC:
        case 0xFD: break;   //NOP
        case 0xFE: return CP_8_imm(A, N8, F);
        case 0xFF: return RST(0x38);

        default: return Instruction{[](){}, 1, 4};
    }
    #undef N8
    #undef N16
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
Instruction CPU::DI(){
    return Instruction{[this](){int_enable = false;}, 1,4};
}
Instruction CPU::EI(){
    return Instruction{[this](){int_enable = true;}, 1,4};
}
}