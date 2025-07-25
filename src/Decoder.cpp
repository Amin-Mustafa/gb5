#include "../include/Decoder.h"
#include "../include/CPU.h"
#include "../include/Register.h"
#include "../include/Instruction.h"

using std::ref;
Decoder::Decoder(CPU& cpu)
    :cpu{cpu},
    regs{ref(cpu.B),ref(cpu.C),ref(cpu.D),ref(cpu.E),ref(cpu.H),ref(cpu.L),ref(cpu.M),ref(cpu.A)}
    {
        reg_pairs.emplace_back(new RegisterPair{cpu.B, cpu.C});
        reg_pairs.emplace_back(new RegisterPair{cpu.D, cpu.E});
        reg_pairs.emplace_back(new RegisterPair{cpu.H, cpu.L});
        reg_pairs.emplace_back(std::make_unique<StackPointer>(cpu.sp));
    }

Instruction Decoder::decode(uint8_t opcode) {
    using namespace Operation;
    using ALUOp = std::function<Instruction(Register8&, const Register8&, FlagRegister&)>;
    static std::vector<ALUOp> alu_ops {
        ADD_8, ADC_8, SUB_8, SBC_8, AND_8, XOR_8, OR_8, CP_8
    };
    if(opcode >= 0x40 && opcode <= 0x7F && opcode != 0x76){
        auto& dest = regs[(opcode - 0x40)/8];
        auto& src = regs[(opcode & 0x0f)%8];
        return LD_8(dest, src);
    }
    if(opcode >= 0x80 && opcode <= 0xBF) {
        ALUOp& op = alu_ops[(opcode-0x80)/8];
        auto& arg = regs[(opcode & 0x0f)%8];
        return op(cpu.A, arg, cpu.F);
    }
    switch(opcode) {
        case 0x00:
        case 0xD3: case 0xDB: case 0xDD:
        case 0xE3: case 0xE4: case 0xEB: case 0xEC: case 0xED:
        case 0xF4: case 0xFC: case 0xFD:
            return Instruction();

        case 0x01: case 0x11: case 0x21: case 0x31:
            return LD_16(*reg_pairs[opcode>>4], Immediate16(cpu));
        
        case 0x02: 
            return LD_8(cpu.A, MemRegister{cpu, cpu.B, cpu.C});
        case 0x12:
            return LD_8(cpu.A, MemRegister{cpu, cpu.D, cpu.E});
        case 0x22: 
            return Instruction{
                [&](){
                    uint16_t hl = RegisterPair{cpu.H, cpu.L}.get();
                    LD_8(cpu.A, cpu.M).execute();
                    hl++;
                    RegisterPair{cpu.H, cpu.L}.set(hl);
                }
            };
        case 0x32: 
            return Instruction{
                [&](){
                    uint16_t hl = RegisterPair{cpu.H, cpu.L}.get();
                    LD_8(cpu.A, cpu.M).execute();
                    hl--;
                    RegisterPair{cpu.H, cpu.L}.set(hl);
                }
            }; 

        case 0x03: case 0x13: case 0x23: case 0x33:
                return INC_16(*reg_pairs[opcode >> 4]);
            
        case 0x04: case 0x0C:
        case 0x14: case 0x1C:
        case 0x24: case 0x2C: 
        case 0x34: case 0x3C:
            return INC_8(regs[(opcode-4)/8], cpu.F);

        case 0x05: case 0x0D:
        case 0x15: case 0x1D:
        case 0x25: case 0x2D: 
        case 0x35: case 0x3D:
            return DEC_8(regs[(opcode-6)/8], cpu.F);

        case 0x06: case 0x0E:
        case 0x16: case 0x1E:
        case 0x26: case 0x2E: 
        case 0x36: case 0x3E:
            return LD_8(regs[(opcode-6)/8], Immediate8(cpu)); 
    }
}