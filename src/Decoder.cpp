#include "../include/Decoder.h"
#include "../include/CPU.h"
#include "../include/Instruction.h"
#include "../include/Arithmetic.h"

using std::ref;
using namespace Operation;

bool NZ(const uint8_t& f) {return !Arithmetic::bit_check(f, (int)Flag::ZERO);}
bool Z (const uint8_t& f) {return  Arithmetic::bit_check(f, (int)Flag::ZERO);}
bool NC(const uint8_t& f) {return !Arithmetic::bit_check(f, (int)Flag::CARRY);}
bool C (const uint8_t& f) {return  Arithmetic::bit_check(f, (int)Flag::CARRY);}
bool no_cond(const uint8_t& f) {return true;}


Decoder::Decoder(CPU& cpu)
    {
        init_instruction_table(cpu);
        init_cb_table(cpu);
    }

Instruction Decoder::decode(uint8_t opcode) {
    return inst_table[opcode];
}

void Decoder::init_instruction_table(CPU& cpu) {
    using namespace Operation;

    inst_table[0x00] = NOP();
    inst_table[0x01] = LD_rr_n16(cpu.B, cpu.C);
    inst_table[0x02] = LD_m_r(cpu.B, cpu.C, cpu.A);
    inst_table[0x03] = INC_rr(cpu.B, cpu.C);
    inst_table[0x04] = INC_r(cpu.B);
    inst_table[0x05] = DEC_r(cpu.B);
    inst_table[0x06] = LD_r_n(cpu.B);
    inst_table[0x07] = ROT_Inst_A(Arithmetic::rot_left_circ);
    inst_table[0x08] = LD_a16_SP();
    inst_table[0x09] = ADD_HL_rr(cpu.B, cpu.C);
    inst_table[0x0A] = LD_r_m(cpu.A, cpu.B, cpu.C);
    inst_table[0x0B] = DEC_rr(cpu.B, cpu.C);
    inst_table[0x0C] = INC_r(cpu.C);
    inst_table[0x0D] = DEC_r(cpu.C);
    inst_table[0x0E] = LD_r_n(cpu.C);
    inst_table[0x0F] = ROT_Inst_A(Arithmetic::rot_right_circ);

    inst_table[0x10] = NOP();   //TODO: STOP instruction
    inst_table[0x11] = LD_16(DE, imm16);
    inst_table[0x12] = LD_8(DE_mem, cpu.A);
    inst_table[0x13] = INC_16(DE);
    inst_table[0x14] = INC_8(cpu.D, cpu.F);
    inst_table[0x15] = DEC_8(cpu.D, cpu.F);
    inst_table[0x16] = LD_8(cpu.D, imm8);
    inst_table[0x17] = Instruction {
        [&]() {
            RL(cpu.A, cpu.F).execute();
            cpu.F.set_flag(Flag::ZERO, 0);
        }
    };
    inst_table[0x18] = cpu.JR(imm8);
    inst_table[0x19] = ADD_16(HL, DE, cpu.F);
    inst_table[0x1A] = LD_8(cpu.A, DE_mem);
    inst_table[0x1B] = DEC_16(DE);
    inst_table[0x1C] = INC_8(cpu.E, cpu.F);
    inst_table[0x1D] = DEC_8(cpu.E, cpu.F);
    inst_table[0x1E] = LD_8(cpu.E, imm8);
    inst_table[0x1F] = Instruction {
        [&](){
            RR(cpu.A, cpu.F).execute();
            cpu.F.set_flag(Flag::ZERO, 0);
        }
    };
    inst_table[0x20] = cpu.JR(imm8, NZ);
    inst_table[0x21] = LD_16(HL, imm16);
    inst_table[0x22] = Instruction {
        [&]() {
            LD_8(cpu.M, cpu.A).execute();
            HL.set(HL.get() + 1);
        } 
    };
    inst_table[0x23] = INC_16(HL);
    inst_table[0x24] = INC_8(cpu.H, cpu.F);
    inst_table[0x25] = DEC_8(cpu.H, cpu.F);
    inst_table[0x26] = LD_8(cpu.H, imm8);
    inst_table[0x27] = NOP();   //TODO DAA
    inst_table[0x28] = cpu.JR(imm8, Z);
    inst_table[0x29] = ADD_16(HL, HL, cpu.F);
    inst_table[0x2A] = Instruction {
        [&]() {
            LD_8(cpu.A, cpu.M).execute();
            HL.set(HL.get() + 1);
        }
    };
    inst_table[0x2B] = DEC_16(HL);
    inst_table[0x2C] = INC_8(cpu.L, cpu.F);
    inst_table[0x2D] = DEC_8(cpu.L, cpu.F);
    inst_table[0x2E] = LD_8(cpu.L, imm8);
    inst_table[0x2F] = Instruction {
        [&](){
            cpu.A.set(~cpu.A.get());
            cpu.F.set_flag(Flag::NEGATIVE, 1);
            cpu.F.set_flag(Flag::HALF_CARRY, 1);
        }
    };

    inst_table[0x30] = cpu.JR(imm8, NC);
    inst_table[0x31] = LD_16(cpu.sp, imm16);
    inst_table[0x32] = Instruction {
        [&]() {
            LD_8(cpu.M, cpu.A).execute();
            HL.set(HL.get() - 1);
        } 
    };
    inst_table[0x33] = INC_16(cpu.sp);
    inst_table[0x34] = INC_8(cpu.M, cpu.F);
    inst_table[0x35] = DEC_8(cpu.M, cpu.F);
    inst_table[0x36] = LD_8(cpu.M, imm8);
    inst_table[0x37] = Instruction{
        [&](){
            bool zero = cpu.F.get_flag(Flag::ZERO);
            cpu.F.set(flag_state(zero, 0, 0, 1));
        }
    };
    inst_table[0x38] = cpu.JR(imm8, C);
    inst_table[0x39] = ADD_16(HL, cpu.sp, cpu.F);
    inst_table[0x3A] = Instruction {
        [&]() {
            LD_8(cpu.A, cpu.M).execute();
            HL.set(HL.get() - 1);
        }
    };
    inst_table[0x3B] = DEC_16(cpu.sp);
    inst_table[0x3C] = INC_8(cpu.A, cpu.F);
    inst_table[0x3D] = DEC_8(cpu.A, cpu.F);
    inst_table[0x3E] = LD_8(cpu.A, imm8);
    inst_table[0x3F] = Instruction {
        [&](){
            bool zero = cpu.F.get_flag(Flag::ZERO);
            bool carry = cpu.F.get_flag(Flag::CARRY);
            cpu.F.set(flag_state(zero, 0, 0, !carry));
        }
    };

    for(int i = 0x40; i < 0x80; ++i) {
        inst_table[i] = LD_8(regs[(i - 0x40)/8], regs[(i & 0x0f)%8]);
    }

    for(int i = 0x80; i < 0xC0; ++i) {
        inst_table[i] = alu_ops[(i - 0x80)/8](cpu.A, regs[(i & 0x0f)%8], cpu.F);
    }

    inst_table[0xC0] = cpu.RET_IF(NZ);
    inst_table[0xC1] = cpu.POP(BC);
    inst_table[0xC2] = cpu.JP(imm16, NZ);
    inst_table[0xC3] = cpu.JP(imm16);
    inst_table[0xC4] = cpu.CALL(imm16, NZ);
    inst_table[0xC5] = cpu.PUSH(BC);
    inst_table[0xC6] = ADD_8(cpu.A, imm8, cpu.F);
    inst_table[0xC7] = cpu.RST(0x00);
    inst_table[0xC8] = cpu.RET_IF(Z);
    inst_table[0xC9] = cpu.RET();
    inst_table[0xCA] = cpu.JP(imm16, Z);
    inst_table[0xCB] = Instruction {
        [this]() {
            cb_table[Immediate8(cpu).get()].execute();
        }
    };
    inst_table[0xCC] = cpu.CALL(imm16, Z);
    inst_table[0xCD] = cpu.CALL(imm16);
    inst_table[0xCE] = ADC_8(cpu.A, imm8, cpu.F);
    inst_table[0xCF] = cpu.RST(0x08);

    inst_table[0xD0] = cpu.RET_IF(NC);
    inst_table[0xD1] = cpu.POP(DE);
    inst_table[0xD2] = cpu.JP(imm16, NC);
    inst_table[0xD3] = NOP();
    inst_table[0xD4] = cpu.CALL(imm16, NC);
    inst_table[0xD5] = cpu.PUSH(DE);
    inst_table[0xD6] = SUB_8(cpu.A, imm8, cpu.F);
    inst_table[0xD7] = cpu.RST(0x10);
    inst_table[0xD8] = cpu.RET_IF(C);
    inst_table[0xD9] = cpu.RETI();
    inst_table[0xDA] = cpu.JP(imm16, C);
    inst_table[0xDB] = NOP();  
    inst_table[0xDC] = cpu.CALL(imm16, C);
    inst_table[0xDD] = NOP();
    inst_table[0xDE] = SBC_8(cpu.A, imm8, cpu.F);
    inst_table[0xDF] = cpu.RST(0x18);

    inst_table[0xE0] = Instruction {
        [&](){
            cpu.write_memory(0xFF00 + cpu.read_memory(cpu.pc+1), cpu.A.get());
            cpu.pc++;
        }
    };
    inst_table[0xE1] = cpu.POP(HL);
    inst_table[0xE2] = Instruction {
        [&](){
            cpu.write_memory(0xFF00 + cpu.C.get(), cpu.A.get());
        }
    };
    inst_table[0xE3] = NOP();
    inst_table[0xE4] = NOP();
    inst_table[0xE5] = cpu.PUSH(HL);
    inst_table[0xE6] = AND_8(cpu.A, imm8, cpu.F);
    inst_table[0xE7] = cpu.RST(0x20);
    inst_table[0xE8] = ADD_SP_e8(cpu.sp, imm8, cpu.F);
    inst_table[0xE9] = cpu.JPHL();
    inst_table[0xEA] = Instruction {
        [&](){
            uint16_t addr = imm16.get();
            cpu.write_memory(addr, cpu.A.get());
        }
    };
    inst_table[0xEB] = NOP();  
    inst_table[0xEC] = NOP();
    inst_table[0xED] = NOP();
    inst_table[0xEE] = XOR_8(cpu.A, imm8, cpu.F);
    inst_table[0xEF] = cpu.RST(0x28);

    inst_table[0xF0] = Instruction {
        [&](){
            cpu.A.set( cpu.read_memory(0xFF00 + cpu.read_memory(cpu.pc + 1)) );
            cpu.pc++;
        }
    };
    inst_table[0xF1] = cpu.POP(AF);
    inst_table[0xF2] = Instruction {
        [&cpu = cpu](){
            cpu.A.set( cpu.read_memory(0xFF00 + cpu.C.get()) );
        }
    };
    inst_table[0xF3] = cpu.DI();    
    inst_table[0xF4] = NOP();
    inst_table[0xF5] = cpu.PUSH(AF);
    inst_table[0xF6] = OR_8(cpu.A, imm8, cpu.F);
    inst_table[0xF7] = cpu.RST(0x30);
    inst_table[0xF8] = Instruction {
        [&](){
            StackPointer temp = cpu.sp;
            ADD_SP_e8(temp, imm8, cpu.F).execute();
            HL.set(temp.get());
        }, 
        -4  //this operation takes 4 fewer cycles than ADD SP+e8 for some reason
    };
    inst_table[0xF9] = Instruction {
        [&]() {
            cpu.sp.set(HL.get());
        },
        4
    };
    inst_table[0xFA] = Instruction {
        [&](){
            uint16_t addr = imm16.get();
            cpu.A.set( cpu.read_memory(addr) );
        }
    };
    inst_table[0xFB] = cpu.EI();
    inst_table[0xFC] = NOP();
    inst_table[0xFD] = NOP();
    inst_table[0xFE] = CP_8(cpu.A, imm8, cpu.F);
    inst_table[0xFF] = cpu.RST(0x38);
}

void Decoder::init_cb_table() {
    for(int i = 0x00; i < 0x40; ++i) {
        cb_table[i] = shift_ops[i / 8](regs[i % 8], cpu.F);
    }
    for(int i = 0x40; i < 0x80; ++i) {
        cb_table[i] = BIT(regs[i % 8], (i - 0x40) / 8, cpu.F);
    }
    for(int i = 0x80; i < 0xC0; ++i) {
        cb_table[i] = RES(regs[i % 8], (i - 0x80) / 8);
    }
    for(int i = 0xC0; i < 0x100; ++i) {
        cb_table[i] = SET(regs[i % 8], (i - 0x80) / 8);
    }
}