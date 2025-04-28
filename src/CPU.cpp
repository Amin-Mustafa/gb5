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
        case 0x01: break;
        case 0x02: return Instruction{load8, {memory[pair(B,C)], A}, 1, 8, "LD [BC],A"};
        case 0x03: break;
        case 0x04: break;
        case 0x05: break;
        case 0x06: return Instruction{load8, {B, byte1}, 2, 8, "LD B,n8"};
        case 0x07: break;
        case 0x08: break;
        case 0x09: break;
        case 0x0A: return Instruction{load8, {A, memory[pair(B,C)]}, 1,8, "LD A,[BC]"};
        case 0x0B: break;
        case 0x0C: break;
        case 0x0D: break;
        case 0x0E: return Instruction{load8, {C, memory[byte1]}, 2,8, "LD C,n8"};
        case 0x0F: break;

        case 0x10: break;
        case 0x11: break;
        case 0x12: return Instruction{load8, {memory[pair(D,E)], A}, 1, 8, "LD [DE],A"};
        case 0x13: break;
        case 0x14: break;
        case 0x15: break;
        case 0x16: return Instruction{load8, {D, byte1}, 2, 8, "LD D,n8"};
        case 0x17: break;
        case 0x18: break;
        case 0x19: break;
        case 0x1A: return Instruction{load8, {A, memory[pair(D,E)]}, 1,8, "LD A,[DE]"};
        case 0x1B: break;
        case 0x1C: break;
        case 0x1D: break;
        case 0x1E: return Instruction{load8, {E, memory[byte1]}, 2,8, "LD E,n8"};
        case 0x1F: break;

        case 0x20: break;
        case 0x21: break;
        case 0x22: {
            Instruction inst{load8, {memory[pair(H,L)], A}, 1,8, "LD A,[HL+]"}; 
            memory[pair(H,L)]++;
            return inst;
        }
        case 0x23: break;
        case 0x24: break;
        case 0x25: break;
        case 0x26: return Instruction{load8, {H, byte1}, 2, 8, "LD H,n8"};
        case 0x27: break;
        case 0x28: break;
        case 0x29: break;
        case 0x2A: {
            Instruction inst{load8, {A, memory[pair(H,L)]}, 1,8, "LD A,[HL+]"}; 
            memory[pair(H,L)]++;
            return inst;
        }
        case 0x2B: break;
        case 0x2C: break;
        case 0x2D: break;
        case 0x2E: return Instruction{load8, {L, memory[byte1]}, 2,8, "LD L,n8"};
        case 0x2F: break;

        case 0x30: break;
        case 0x31: break;
        case 0x32: {
            Instruction inst{load8, {memory[pair(H,L)], A}, 1,8, "LD A,[HL-]"}; 
            memory[pair(H,L)]--;
            return inst;
        }
        case 0x33: break;
        case 0x34: break;
        case 0x35: break;
        case 0x36: return Instruction{load8, {memory[pair(H,L)], byte1}, 2, 8, "LD [HL],n8"};
        case 0x3A: {
            Instruction inst{load8, {A, memory[pair(H,L)]}, 1,8, "LD A,[HL-]"}; 
            memory[pair(H,L)]--;
            return inst;
        }
        case 0x3B: break;
        case 0x3C: break;
        case 0x3D: break;
        case 0x3E: return Instruction{load8, {A, memory[byte1]}, 2,8, "LD A,n8"};
        case 0x3F: break;

        case 0x40: return Instruction{load8, {B, B}, 1,4, "LD B,B"};
        case 0x41: return Instruction{load8, {B, C}, 1,4, "LD B,C"};
        case 0x42: return Instruction{load8, {B, D}, 1,4, "LD B,D"};
        case 0x43: return Instruction{load8, {B, E}, 1,4, "LD B,E"};
        case 0x44: return Instruction{load8, {B, H}, 1,4, "LD B,H"};
        case 0x45: return Instruction{load8, {B, L}, 1,4, "LD B,L"};
        case 0x46: return Instruction{load8, {B, memory[pair(H,L)]}, 1,4, "LD B,[HL]"};
        case 0x47: return Instruction{load8, {B, A}, 1,4, "LD B,A"};
        
        case 0x48: return Instruction{load8, {C, B}, 1,4, "LD C,B"};
        case 0x49: return Instruction{load8, {C, C}, 1,4, "LD C,C"};
        case 0x4A: return Instruction{load8, {C, D}, 1,4, "LD C,D"};
        case 0x4B: return Instruction{load8, {C, E}, 1,4, "LD C,E"};
        case 0x4C: return Instruction{load8, {C, H}, 1,4, "LD C,H"};
        case 0x4D: return Instruction{load8, {C, L}, 1,4, "LD C,L"};
        case 0x4E: return Instruction{load8, {C, memory[pair(H,L)]}, 1,4, "LD C,[HL]"};
        case 0x4F: return Instruction{load8, {C, A}, 1,4, "LD C,A"};

        case 0x50: return Instruction{load8, {B, B}, 1,4, "LD D,B"};
        case 0x51: return Instruction{load8, {D, C}, 1,4, "LD D,C"};
        case 0x52: return Instruction{load8, {D, D}, 1,4, "LD D,D"};
        case 0x53: return Instruction{load8, {D, E}, 1,4, "LD D,E"};
        case 0x54: return Instruction{load8, {D, H}, 1,4, "LD D,H"};
        case 0x55: return Instruction{load8, {D, L}, 1,4, "LD D,L"};
        case 0x56: return Instruction{load8, {D, memory[pair(H,L)]}, 1,4, "LD D,[HL]"};
        case 0x57: return Instruction{load8, {D, A}, 1,4, "LD D,A"};

        case 0x58: return Instruction{load8, {E, B}, 1,4, "LD E,B"};
        case 0x59: return Instruction{load8, {E, C}, 1,4, "LD E,C"};
        case 0x5A: return Instruction{load8, {E, D}, 1,4, "LD E,D"};
        case 0x5B: return Instruction{load8, {E, E}, 1,4, "LD E,E"};
        case 0x5C: return Instruction{load8, {E, H}, 1,4, "LD E,H"};
        case 0x5D: return Instruction{load8, {E, L}, 1,4, "LD E,L"};
        case 0x5E: return Instruction{load8, {E, memory[pair(H,L)]}, 1,4, "LD E,[HL]"};
        case 0x5F: return Instruction{load8, {E, A}, 1,4, "LD E,A"};

        case 0x60: return Instruction{load8, {H, B}, 1,4, "LD H,B"};
        case 0x61: return Instruction{load8, {H, C}, 1,4, "LD H,C"};
        case 0x62: return Instruction{load8, {H, D}, 1,4, "LD H,D"};
        case 0x63: return Instruction{load8, {H, E}, 1,4, "LD H,E"};
        case 0x64: return Instruction{load8, {H, H}, 1,4, "LD H,H"};
        case 0x65: return Instruction{load8, {H, L}, 1,4, "LD H,L"};
        case 0x66: return Instruction{load8, {H, memory[pair(H,L)]}, 1,4, "LD H,[HL]"};
        case 0x67: return Instruction{load8, {H, A}, 1,4, "LD H,A"};
        
        case 0x68: return Instruction{load8, {L, B}, 1,4, "LD L,B"};
        case 0x69: return Instruction{load8, {L, C}, 1,4, "LD L,C"};
        case 0x6A: return Instruction{load8, {L, D}, 1,4, "LD L,D"};
        case 0x6B: return Instruction{load8, {L, E}, 1,4, "LD L,E"};
        case 0x6C: return Instruction{load8, {L, H}, 1,4, "LD L,H"};
        case 0x6D: return Instruction{load8, {L, L}, 1,4, "LD L,L"};
        case 0x6E: return Instruction{load8, {L, memory[pair(H,L)]}, 1,4, "LD L,[HL]"};
        case 0x6F: return Instruction{load8, {L, A}, 1,4, "LD L,A"};

        case 0x70: return Instruction{load8, {memory[pair(H,L)], B}, 1,4, "LD [HL],B"};
        case 0x71: return Instruction{load8, {memory[pair(H,L)], C}, 1,4, "LD [HL],C"};
        case 0x72: return Instruction{load8, {memory[pair(H,L)], D}, 1,4, "LD [HL],D"};
        case 0x73: return Instruction{load8, {memory[pair(H,L)], E}, 1,4, "LD [HL],E"};
        case 0x74: return Instruction{load8, {memory[pair(H,L)], H}, 1,4, "LD [HL],H"};
        case 0x75: return Instruction{load8, {memory[pair(H,L)], L}, 1,4, "LD [HL],L"};
        case 0x76: break;   //HALT
        case 0x77: return Instruction{load8, {memory[pair(H,L)], A}, 1,4, "LD [HL],A"};

        case 0x78: return Instruction{load8, {A, B}, 1,4, "LD A,B"};
        case 0x79: return Instruction{load8, {A, C}, 1,4, "LD A,C"};
        case 0x7A: return Instruction{load8, {A, D}, 1,4, "LD A,D"};
        case 0x7B: return Instruction{load8, {A, E}, 1,4, "LD A,E"};
        case 0x7C: return Instruction{load8, {A, H}, 1,4, "LD A,H"};
        case 0x7D: return Instruction{load8, {A, L}, 1,4, "LD A,L"};
        case 0x7E: return Instruction{load8, {A, memory[pair(H,L)]}, 1,4, "LD A,[HL]"};
        case 0x7F: return Instruction{load8, {A, A}, 1,4, "LD A,A"};

        case 0x80: break;
        case 0x81: break;
        case 0x82: break;
        case 0x83: break;
        case 0x84: break;
        case 0x85: break;
        case 0x86: break;
        case 0x87: break;
        case 0x88: break;
        case 0x89: break;
        case 0x8A: break;
        case 0x8B: break;
        case 0x8C: break;
        case 0x8D: break;
        case 0x8E: break;
        case 0x8F: break;

        case 0x90: break;
        case 0x91: break;
        case 0x92: break;
        case 0x93: break;
        case 0x94: break;
        case 0x95: break;
        case 0x96: break;
        case 0x97: break;
        case 0x98: break;
        case 0x99: break;
        case 0x9A: break;
        case 0x9B: break;
        case 0x9C: break;
        case 0x9D: break;
        case 0x9E: break;
        case 0x9F: break;

        case 0xA0: break;
        case 0xA1: break;
        case 0xA2: break;
        case 0xA3: break;
        case 0xA4: break;
        case 0xA5: break;
        case 0xA6: break;
        case 0xA7: break;
        case 0xA8: break;
        case 0xA9: break;
        case 0xAA: break;
        case 0xAB: break;
        case 0xAC: break;
        case 0xAD: break;
        case 0xAE: break;
        case 0xAF: break;

        case 0xB0: break;
        case 0xB1: break;
        case 0xB2: break;
        case 0xB3: break;
        case 0xB4: break;
        case 0xB5: break;
        case 0xB6: break;
        case 0xB7: break;
        case 0xB8: break;
        case 0xB9: break;
        case 0xBA: break;
        case 0xBB: break;
        case 0xBC: break;
        case 0xBD: break;
        case 0xBE: break;
        case 0xBF: break;

        case 0xC0: break;
        case 0xC1: break;
        case 0xC2: break;
        case 0xC3: break;
        case 0xC4: break;
        case 0xC5: break;
        case 0xC6: break;
        case 0xC7: break;
        case 0xC8: break;
        case 0xC9: break;
        case 0xCA: break;
        case 0xCB: break;
        case 0xCC: break;
        case 0xCD: break;
        case 0xCE: break;
        case 0xCF: break;

        case 0xD0: break;
        case 0xD1: break;
        case 0xD2: break;
        case 0xD3: break;
        case 0xD4: break;
        case 0xD5: break;
        case 0xD6: break;
        case 0xD7: break;
        case 0xD8: break;
        case 0xD9: break;
        case 0xDA: break;
        case 0xDB: break;
        case 0xDC: break;
        case 0xDD: break;
        case 0xDE: break;
        case 0xDF: break;

        case 0xE0: return Instruction{load8, {memory[pair(0xff,byte1)], A}, 2,12, "LDH [a8],A"};
        case 0xE1: break;
        case 0xE2: return Instruction{load8, {memory[pair(0xff,C)], A}, 1, 8, "LDH [C],A"};
        case 0xE3: break;
        case 0xE4: break;
        case 0xE5: break;
        case 0xE6: break;
        case 0xE7: break;
        case 0xE8: break;
        case 0xE9: break;
        case 0xEA: break;
        case 0xEB: break;
        case 0xEC: break;
        case 0xED: break;
        case 0xEE: break;
        case 0xEF: break;

        case 0xF0: return Instruction{load8, {A, memory[pair(0xff,byte1)]}, 2,12, "LDH A,[a8]"};
        case 0xF1: break;
        case 0xF2: return Instruction{load8, {A, memory[pair(0xff,C)]}, 1, 8, "LDH A,[C]"};
        case 0xF3: break;
        case 0xF4: break;
        case 0xF5: break;
        case 0xF6: break;
        case 0xF7: break;
        case 0xF8: break;
        case 0xF9: break;
        case 0xFA: break;
        case 0xFB: break;
        case 0xFC: break;
        case 0xFD: break;
        case 0xFE: break;
        case 0xFF: break;

        default: return Instruction{nop, {}, 1, 4, "NOP"};
    }
}

void CPU::fetch_and_execute() {
    Instruction inst = decode(memory[pc]);
    inst.print();
    inst.execute(*this);
    pc += inst.length();
}
}