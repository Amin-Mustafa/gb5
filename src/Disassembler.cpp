#include "../include/Disassembler.h"
#include "../include/MMU.h"

#include <iostream>
#include <iomanip>
#include <format>

namespace SM83 {

void Disassembler::disassemble_at(uint16_t pos) {
    uint8_t opcode = mem[pos];
    static std::string registers[] = {"B", "C", "D", "E", "H", "L", "[HL]", "A"};
    static std::string reg_pairs[] = {"BC", "DE", "HL", "SP"};
    static std::string mem_pairs[] = {"[BC]", "[DE]", "[HL+]", "[HL-]"};
    static std::string control_conditions[] = {"NZ", "Z", "NC", "C"};
    static std::string alu_ops[] = {"ADD", "ADC", "SUB", "SBC", "AND", "XOR", "OR", "CP"};

    using std::cout;
    using std::format;
    #define BYTE1 mem[pos+1]
    #define BYTE2 mem[pos+2]

    cout << format("PC: ${:04x} 0x{:02x}\t", pos, opcode);

    cout << std::left << std::setw(10);

    //register loads
    if(opcode >= 0x40 && opcode <= 0x7F && opcode != 0x76){
        cout << format("LD {}, {}", registers[(opcode - 0x40)/8], registers[(opcode & 0x0f)%8])
             << '\t';
        return;
    }
    //alu ops
    if(opcode >= 0x80 && opcode <= 0xBF) {
        cout << format("{} A, {}", alu_ops[(opcode-0x80)/8], registers[(opcode & 0x0f)%8])
             << '\t';
        return;
    }
    
    //everything else
    switch(opcode){
        case 0x00:
        case 0xD3: case 0xDB: case 0xDD:
        case 0xE3: case 0xE4: case 0xEB: case 0xEC: case 0xED:
        case 0xF4: case 0xFC: case 0xFD:
            cout << "NOP";
            break;

        case 0x01: case 0x11: case 0x21: case 0x31:
            cout << format("LD {}, ${:02x}{:02x}", reg_pairs[opcode>>4], BYTE2, BYTE1); 
            break;

        case 0x02: case 0x12: case 0x22: case 0x32:
            cout << format("LD {}, A", mem_pairs[opcode>>4]);
            break;

        case 0x03: case 0x13: case 0x23: case 0x33:
            cout << format("INC {}", reg_pairs[opcode>>4]);
            break;

        case 0x04: case 0x0C:
        case 0x14: case 0x1C:
        case 0x24: case 0x2C: 
        case 0x34: case 0x3C:
            cout << format("INC {}", registers[(opcode-4)/8]);
            break;

        case 0x05: case 0x0D:
        case 0x15: case 0x1D:
        case 0x25: case 0x2D: 
        case 0x35: case 0x3D:
            cout << format("DEC {}", registers[(opcode-5)/8]);
            break;

        case 0x06: case 0x0E:
        case 0x16: case 0x1E:
        case 0x26: case 0x2E: 
        case 0x36: case 0x3E:
            cout << format("LD {}, ${:02x}", registers[(opcode-6)/8], BYTE1);
            break;

        case 0x07: cout << "RLCA"; break;

        case 0x08: cout << format("[${:02x}{:02x}], SP", BYTE2, BYTE1); break;
        
        case 0x09: case 0x19: case 0x29: case 0x39:
            cout << format("ADD HL, {}", reg_pairs[opcode>>4]);
            break;
        
        case 0x0A: case 0x1A: case 0x2A: case 0x3A:
            cout << format("LD A, {}", mem_pairs[opcode>>4]);
            break;

        case 0x0B: case 0x1B: case 0x2B: case 0x3B:
            cout << format("DEC {}", reg_pairs[opcode>>4]);
            break;

        case 0x0F: cout << "RRCA"; break;
        case 0x1F: cout << "RRA"; break;
        case 0x2F: cout << "CPL"; break;
        case 0x3F: cout << "CCF"; break;

        case 0x18: cout << format("JR ${:02x}", BYTE1); break;
        case 0x20: case 0x28: case 0x30: case 0x38:
            cout << format("JR {} ${:02x}", control_conditions[(opcode - 0x20)/8], BYTE1);
            break;

        case 0xC9: cout << "RET"; break;
        case 0xC0: case 0xC8: case 0xD0: case 0xD8:
            cout << format("RET {}", control_conditions[(opcode - 0xC0)/8]);
            break;
        case 0xD9: cout << "RETI"; break;
        
        case 0xC3: cout << format("JP ${:02x}{:02x}", BYTE2, BYTE1); break;
        case 0xC2: case 0xCA: case 0xD2: case 0xDA:
            cout << format("JP {}, ${:02x}{:02x}", control_conditions[(opcode - 0xC2)/8], BYTE2, BYTE1);
            break; 

        case 0xCD: cout << format("CALL ${:02x}{:02x}", BYTE2, BYTE1); break;
        case 0xC4: case 0xCC: case 0xD4: case 0xDC:
            cout << format("CALL {}, ${:02x}{:02x}", control_conditions[(opcode - 0xC4)/8], BYTE2, BYTE1);
            break;

        case 0xC1: case 0xD1: case 0xE1: case 0xF1:
            cout << format("POP {}", reg_pairs[(opcode>>4) - 0xC]);
            break;

        case 0xC5: case 0xD5: case 0xE5: case 0xF5:
            cout << format("PUSH {}", reg_pairs[(opcode>>4) - 0xC]);
            break;
        
        case 0xC6: case 0xD6: case 0xE6: case 0xF6:
        case 0xCE: case 0xDE: case 0xEE: case 0xFE:
            cout << format("{} A, ${:02x}", alu_ops[(opcode - 0xC6)/8], BYTE1);
            break;

        case 0xC7: case 0xD7: case 0xE7: case 0xF7:
        case 0xCF: case 0xDF: case 0xEF: case 0xFF:
            cout << format("RST ${:02x}", opcode - 0xC7);
            break;
        
        case 0xE0: cout << format("LDH  [${:02x}], A", BYTE1);    break;
        case 0xE2: cout << format("LDH [C], A");    break;
        case 0xF0: cout << format("LDH A,  [${:02x}]", BYTE1);    break;
        case 0xF2: cout << format("LDH A, [C]");    break;

        case 0xE8: cout << format("ADD SP, ${:02x}", BYTE1); break;

        case 0xEA: cout << format("LD [${:02x}{:02x}], A", BYTE2, BYTE1);    break;
        case 0xFA: cout << format("LD A, [${:02x}{:02x}]", BYTE2, BYTE1);    break;

        case 0xF8: cout << format("ADD HL, SP + ${:02x}", BYTE1);    break;
        case 0xF9: cout << format("LD SP, HL");

        case 0xF3: cout << "DI"; break;
        case 0xFB: cout << "EI"; break;
        default: cout << "UNIMPLEMENTED INSTRUCTION"; break;
    }
    cout << "\t";

    #undef BYTE2
    #undef BYTE1
}

}