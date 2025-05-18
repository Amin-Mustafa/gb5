#include "../include/CPU.h"
#include "../include/Instruction.h"
#include "../include/MMU.h"
#include <iostream>
#include <format>

namespace SM83 {

constexpr uint16_t pair(uint8_t hi, uint8_t lo) {
    return (hi << 8) | lo;
}

constexpr uint8_t flag_state(bool z, bool n, bool h, bool c) {
    return z << 7 | n << 6 | h << 5 | c << 4;
}

Instruction CPU::jr(uint8_t offset, bool condition, std::string name) {
    if(condition) {
        return Instruction{[this, offset](){ pc += offset; }, 2, 12, name};
    }
    else return Instruction{[](){}, 2,8, name};
}

Instruction CPU::decode(uint8_t opcode) {
    using std::ref;
    uint8_t& byte1 = memory[pc+1];
    uint8_t& byte2 = memory[pc+2];
    switch(opcode) {
        using namespace Operation;
        case 0x00: return Instruction{[](){}, 1, 4, "NOP"};
        case 0x01: return Instruction{
            Operation::load16(B, C, byte2, byte1), 3,12, std::format("LD BC,{}{:x}",byte2,byte1)
        };
        case 0x02: return Instruction{load8(memory[pair(B,C)], A), 1,8, "LD [BC],A"}; 
        case 0x03: return Instruction{inc16(B,C), 1,8, "INC BC"};
        case 0x04: return Instruction{inc8(B,F), 1,4, "INC B"};
        case 0x05: return Instruction{dec8(B,F), 1,4, "DEC B"};
        case 0x06: return Instruction{load8(B, byte1), 2,8, std::format("LD B,{:x}", byte1)};
        case 0x07: return Instruction{
            [this]() {
                F = flag_state(0,0,0,A>>7);
                A = (A << 1) | (A >> 7);
            }, 1, 4, "RLCA"
        };
        case 0x08: return Instruction{
            [this, byte2, byte1]() {
                memory[pair(byte2,byte1)] = sp & 0xff;
                memory[pair(byte2,byte1) + 1] = sp >> 8;
            }, 3, 20, std::format("LD [{}{}],SP", byte2, byte1) 
        };
        case 0x09: return Instruction{add16(H,L,B,C,F), 1,8, "ADD HL,BC"};
        case 0x0A: return Instruction{load8(A, memory[pair(B,C)]), 1,8, "LD A,[BC]"};
        case 0x0B: return Instruction{dec16(B,C), 1,8, "DEC BC"};
        case 0x0C: return Instruction{inc8(C,F), 1,4, "INC C"};
        case 0x0D: return Instruction{dec8(C,F), 1,4, "DEC C"};
        case 0x0E: return Instruction{load8(C,byte1), 2,8, std::format("LD C,{:x}", byte1)};
        case 0x0F: return Instruction{
            [this]() {
                F = flag_state(0,0,0, A & 1);
                A = (A >> 1) | (A << 7);
            }, 1, 4, "RRCA"
        };

        case 0x10: return Instruction{[](){}, 2, 4, "STOP"};    //TODO
        case 0x11: return Instruction{load16(D,E,byte2,byte1), 3, 12, std::format("LD DE,{:x}", byte1)};
        case 0x12: return Instruction{load8(memory[pair(D,E)], A), 1,8, "LD [DE],A"};
        case 0x13: return Instruction{inc16(D,E), 1,8, "INC DE"};
        case 0x14: return Instruction{inc8(D,F), 1,4, "INC D"};
        case 0x15: return Instruction{dec8(D,F), 1,4, "DEC D"};
        case 0x16: return Instruction{load8(D, byte1), 2,8, std::format("LD D,{:x}", byte1)};
        case 0x17: return Instruction{
            [this](){
                bool old_carry = F.get_flag(Flag::CARRY);
                F = flag_state(0, 0, 0, A >> 7);
                A = ((A << 1) & ~((uint8_t)1)) | old_carry;
            }, 1,4, "RLA"
        };
        case 0x18: return jr(byte1, 1, std::format("JR {:x}", byte1));
        case 0x19: return Instruction{add16(H,L,D,E,F), 1,8, "ADD HL,DE"};
        case 0x1A: return Instruction{load8(A, memory[pair(D,E)]), 1,8, "LD A,[DE]"};
        case 0x1B: return Instruction{dec16(D,E), 1,8, "DEC DE"};
        case 0x1C: return Instruction{inc8(E,F), 1,4, "INC E"};
        case 0x1D: return Instruction{dec8(E,F), 1,4, "DEC E"};
        case 0x1E: return Instruction{load8(E,byte1), 2,8, std::format("LD E,{:x}",byte1)};
        case 0x1F: return Instruction{
            [this](){
                bool old_carry = F.get_flag(Flag::CARRY);
                F = flag_state(0, 0, 0, A & 1);
                A = ((A >> 1) & ~((uint8_t)1 << 7)) | (old_carry << 7);
            }, 1,4, "RRA"
        };
        
        case 0x20: return jr(byte1, !F.get_flag(Flag::ZERO), std::format("JR NZ,{:x}", byte1));
        case 0x21: return Instruction{load16(H,L,byte2,byte1), 3, 12, std::format("LD HL,{:x}", byte1)};
        case 0x22: {
            Instruction inst{load8(memory[pair(H,L)], A), 1,8, "LD [HL+],A"};
            inc16(H,L)();
            return inst;
        };
        case 0x23: return Instruction{inc16(H,L), 1,8, "INC HL"};
        case 0x24: return Instruction{inc8(H,F), 1,4, "INC H"};
        case 0x25: return Instruction{dec8(H,F), 1,4, "DEC H"};
        case 0x26: return Instruction{load8(H, byte1), 2,8, std::format("LD H,{:x}", byte1)};
        case 0x27: return Instruction{[](){}, 1,4, "NOP"};  //TODO: DAA
        case 0x28: return jr(byte1, F.get_flag(Flag::ZERO), std::format("JR Z,{:x}", byte1));
        case 0x29: return Instruction{add16(H,L,H,L,F), 1,8, "ADD HL,HL"};
        case 0x2A: {
            Instruction inst{load8(A, memory[pair(H,L)]), 1,8, "LD A,[HL+]"};
            inc16(H,L)();
            return inst;
        };
        case 0x2B: return Instruction{dec16(H,L), 1,8, "DEC HL"};
        case 0x2C: return Instruction{inc8(L,F), 1,4, "INC L"};
        case 0x2D: return Instruction{dec8(L,F), 1,4, "DEC L"};
        case 0x2E: return Instruction{load8(L,byte1), 2,8, std::format("LD L,{:x}",byte1)};
        case 0x2F: return Instruction{
            [this](){
                A = ~A;
                F.set_flag(Flag::NEGATIVE, 1);
                F.set_flag(Flag::HALF_CARRY, 1);
            }, 1,4, "CPL"
        };

        case 0x30: return jr(byte1, !F.get_flag(Flag::CARRY), std::format("JR NC,{:x}", byte1));
        case 0x31: return Instruction{
            [this,byte2,byte1](){sp = pair(byte2,byte1);}, 
            3, 12, std::format("LD SP,{}{:x}", byte2, byte1)
        };
        case 0x32: {
            Instruction inst{load8(memory[pair(H,L)], A), 1,8, "LD [HL-],A"};
            dec16(H,L)();
            return inst;
        };
        case 0x33: return Instruction{[this](){sp++;}, 1,8, "INC SP"};
        case 0x34: return Instruction{inc8(memory[pair(H,L)], F), 1,12, "INC [HL]"};
        case 0x35: return Instruction{dec8(memory[pair(H,L)], F), 1,12, "DEC [HL]"};
        case 0x36: return Instruction{load8(memory[pair(H,L)], byte1), 2,12, std::format("LD [HL],{:x}",byte1)};
        case 0x37: return Instruction{
            [this](){
                F = flag_state(F.get_flag(Flag::ZERO), 0, 0, 1);
            }, 1,4, "SCF"
        };
        case 0x38: return jr(byte1, F.get_flag(Flag::CARRY), std::format("JR C,{:x}",byte1));
        case 0x39: return Instruction{add16(H,L, sp>>8, sp&0xff, F), 1,8, "ADD HL,SP"};
        case 0x3A: {
            Instruction inst{load8(A, memory[pair(H,L)]), 1,8, "LD A, [HL-]"};
            dec16(H,L)();
            return inst;
        };
        case 0x3B: return Instruction{[this](){sp--;}, 1,8, "DEC SP"};
        case 0x3C: return Instruction{inc8(A,F), 1,4, "INC A"};
        case 0x3D: return Instruction{dec8(A,F), 1,4, "DEC A"};
        case 0x3E: return Instruction{load8(A,byte1), 2,8, std::format("LD A,{:x}",byte1)};
        case 0x3F: return Instruction{
            [this](){
                F = flag_state(F.get_flag(Flag::ZERO), 0, 0, !F.get_flag(Flag::CARRY));
            }, 1,4, "CCF"
        };
        //..
        case 0x80: return Instruction{add8(A,B,0,F), 1,4, "ADD A,B"};
        case 0x90: return Instruction{sub8(A,B,0,F), 1,4, "SUB A,B"};
        default: return Instruction{[](){}, 1, 4, "NOP"};
    }
}

void CPU::fetch_and_execute() {
    Instruction inst = decode(memory[pc]);
    inst.print();
    inst.execute();
    pc += inst.length();
}
}