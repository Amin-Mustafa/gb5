#include "../include/Arithmetic.h"
#include "../include/Instruction.h"
#include "../include/Memory/MMU.h"
#include "../include/CPU.h"

uint16_t pair(uint8_t hi, uint8_t lo) {
    return ((uint16_t)hi << 8 ) | lo;
}

struct Pair {
    uint8_t& hi;
    uint8_t& lo;
    uint16_t get() { return pair(hi, lo); }
    void set(uint16_t val) {
        hi = val >> 8;
        lo = val & 0xFF;
    }
};

uint8_t flag_state(bool z, bool n, bool h, bool c){
    return (z << 7) | (n << 6) | (h << 5) | (c << 4);
}

namespace Operation {

    namespace ALU {
        //primitive arithmetic and logic micro ops
        void add_8(CPU& cpu, uint8_t num) {
            uint16_t result = cpu.A + num;

            cpu.set_flag(Flag::ZERO, (result&0xff) == 0);
            cpu.set_flag(Flag::NEGATIVE, 0);
            cpu.set_flag(Flag::HALF_CARRY, Arithmetic::half_carry_add_8(cpu.A, num));
            cpu.set_flag(Flag::CARRY, result > 0xff);

            cpu.A = result & 0xff;
        }
        void adc_8(CPU& cpu, uint8_t num) {
            bool c = cpu.get_flag(Flag::CARRY);
            uint16_t result = cpu.A + num + c;

            cpu.set_flag(Flag::ZERO, (result&0xff) == 0);
            cpu.set_flag(Flag::NEGATIVE, 0);
            cpu.set_flag(Flag::HALF_CARRY, Arithmetic::half_carry_add_8(cpu.A, num, c));
            cpu.set_flag(Flag::CARRY, result > 0xff);

            cpu.A = result & 0xff;
        }
        void sub_8(CPU& cpu, uint8_t num) {
            uint16_t result = cpu.A - num;

            cpu.set_flag(Flag::ZERO, (result&0xff) == 0);
            cpu.set_flag(Flag::NEGATIVE, 1);
            cpu.set_flag(Flag::HALF_CARRY, Arithmetic::half_carry_sub_8(cpu.A, num));
            cpu.set_flag(Flag::CARRY, num > cpu.A);

            cpu.A = result & 0xff;
        }
        void sbc_8(CPU& cpu, uint8_t num) {
            bool c = cpu.get_flag(Flag::CARRY);
            uint16_t result = cpu.A - num - c;

            cpu.set_flag(Flag::ZERO, (result&0xff) == 0);
            cpu.set_flag(Flag::NEGATIVE, 1);
            cpu.set_flag(Flag::HALF_CARRY, Arithmetic::half_carry_sub_8(cpu.A, num, c));
            cpu.set_flag(Flag::CARRY, num + c > cpu.A);

            cpu.A = result & 0xff;
        }
        void cp_8(CPU& cpu, uint8_t num) {
            uint16_t result = cpu.A - num;

            cpu.set_flag(Flag::ZERO, (result&0xff) == 0);
            cpu.set_flag(Flag::NEGATIVE, 1);
            cpu.set_flag(Flag::HALF_CARRY, Arithmetic::half_carry_sub_8(cpu.A, num));
            cpu.set_flag(Flag::CARRY, num > cpu.A);
        }
        void inc_8(CPU& cpu, uint8_t& num) {
            uint16_t result = num + 1;
            cpu.set_flag(Flag::ZERO, (result&0xff) == 0);
            cpu.set_flag(Flag::NEGATIVE, 0);
            cpu.set_flag(Flag::HALF_CARRY, Arithmetic::half_carry_add_8(num, 1));
            num = result & 0xff;
        }
        void dec_8(CPU& cpu, uint8_t& num) {
            uint16_t result = num - 1;
            cpu.set_flag(Flag::ZERO, (result&0xff) == 0);
            cpu.set_flag(Flag::NEGATIVE, 1);
            cpu.set_flag(Flag::HALF_CARRY, Arithmetic::half_carry_sub_8(num, 1));
            num = result&0xff;
        }
        void and_8(CPU& cpu, uint8_t num) {
            cpu.A = cpu.A & num;
            cpu.F &= flag_state(cpu.A == 0, 0, 1, 0);
        }
        void or_8(CPU& cpu, uint8_t num) {
            cpu.A = cpu.A | num;
            cpu.F &= flag_state(cpu.A == 0, 0, 0, 0);
        }
        void xor_8(CPU& cpu, uint8_t num) {
            cpu.A = cpu.A ^ num;
            cpu.F &= flag_state(cpu.A == 0, 0, 0, 0);
        }
    }

Instruction NOP() {
    return Instruction {
        [](CPU&){}
    };  //do nothing
}

//----------------------LOADS-----------------------//
//------8-bit------//
Instruction LD_r_r(uint8_t& dest, const uint8_t& src) {
    return Instruction { 
        [&dest, &src](CPU&){ dest = src;}
    };
}
Instruction LD_r_n(uint8_t& dest) {
    return Instruction {
        [](CPU& cpu) {cpu.latch.Z = cpu.fetch_byte();},        //M1
        [&dest](CPU& cpu) {dest = cpu.latch.Z;}                //M2
    };
}
Instruction LD_r_m(uint8_t& dest, uint8_t& src_hi, uint8_t& src_lo) {
    return Instruction {
        [&src_hi, &src_lo](CPU& cpu) {
            cpu.latch.Z = cpu.read_memory(pair(src_hi, src_lo));
        },    //M1
        [&dest](CPU& cpu) {dest = cpu.latch.Z;}            //M2
    };
}
Instruction LD_m_r(uint8_t& dest_hi, uint8_t& dest_lo, uint8_t& src) {
    return Instruction {
        [&dest_hi, &dest_lo, &src](CPU& cpu) {
            cpu.write_memory(pair(dest_hi, dest_lo), src);
        },      //M1
        [](CPU& cpu) {/* dummy */}                     //M2
    };
}
Instruction LD_m_n(uint8_t& dest_hi, uint8_t& dest_lo) {
    return Instruction {
        [](CPU& cpu) {cpu.latch.Z = cpu.fetch_byte();},
        [&dest_hi, &dest_lo](CPU& cpu) {
            cpu.write_memory(pair(dest_hi, dest_lo), cpu.latch.Z);
        },
        [](CPU&){}
    };
}
Instruction LD_A_a16() {
    return Instruction {
        [](CPU& cpu) {cpu.latch.Z = cpu.fetch_byte();},  //M1: read lsb of addr
        [](CPU& cpu) {cpu.latch.W = cpu.fetch_byte();},
        [](CPU& cpu) {cpu.latch.Z = cpu.read_memory(cpu.latch.combined());},
        [](CPU& cpu) {cpu.A = cpu.latch.Z;}
    };
}
Instruction LD_a16_A() {
    return Instruction {
        [](CPU& cpu) {cpu.latch.Z = cpu.fetch_byte();},
        [](CPU& cpu) {cpu.latch.W = cpu.fetch_byte();},
        [](CPU& cpu) {cpu.write_memory(cpu.latch.combined(), cpu.A);},
        [](CPU& cpu) {}
    };
}
Instruction LDH_A_C() {
    return Instruction {
        [](CPU& cpu) {cpu.latch.Z = cpu.read_memory(0xFF00 + cpu.C);},
        [](CPU& cpu) {cpu.A = cpu.latch.Z;}
    };
}
Instruction LDH_C_A(){
    return Instruction {
        [](CPU& cpu) {cpu.write_memory(0xFF00 + cpu.C, cpu.A);},
        [](CPU& cpu) {}
    };
}
Instruction LDH_A_n(){
    return Instruction {
        [](CPU& cpu) {cpu.latch.Z = cpu.fetch_byte();},
        [](CPU& cpu) {cpu.latch.Z = cpu.read_memory(0xFF00 + cpu.latch.Z);},
        [](CPU& cpu) {cpu.A = cpu.latch.Z;}
    };
}
Instruction LDH_n_A(){
    return Instruction {
        [](CPU& cpu) {cpu.latch.Z = cpu.fetch_byte();},
        [](CPU& cpu) {cpu.write_memory(0xFF00 + cpu.latch.Z, cpu.A);},
        [](CPU& cpu) {}
    };
}
Instruction LD_A_HLdec() {
    return Instruction {
        [](CPU& cpu) {
            Pair HL{cpu.H, cpu.L};
            cpu.latch.Z = cpu.read_memory(HL.get());
            HL.set(HL.get()-1);
        },
        [](CPU& cpu) {cpu.A = cpu.latch.Z;}
    };
}
Instruction LD_HLdec_A() {
    return Instruction {
        [](CPU& cpu) {
            Pair HL{cpu.H, cpu.L};
            cpu.write_memory(HL.get(), cpu.A);
            HL.set(HL.get() - 1);
        },
        [](CPU& cpu) {}
    };
}
Instruction LD_A_HLinc() {
    return Instruction {
        [](CPU& cpu) {
            Pair HL{cpu.H, cpu.L};
            cpu.latch.Z = cpu.read_memory(HL.get());
            HL.set(HL.get()+1);
        },
        [](CPU& cpu) {cpu.A = cpu.latch.Z;}
    };
}
Instruction LD_HLinc_A() {
    return Instruction {
        [](CPU& cpu) {
            Pair HL{cpu.H, cpu.L};
            cpu.write_memory(HL.get(), cpu.A);
            HL.set(HL.get() + 1);
        },
        [](CPU& cpu) {}
    };
}

//--------16-bit--------//
Instruction LD_rr_n16(uint8_t& dest_hi, uint8_t& dest_lo) {
    return Instruction {
        [](CPU& cpu) {cpu.latch.Z = cpu.fetch_byte();},
        [](CPU& cpu) {cpu.latch.W = cpu.fetch_byte();},
        [&dest_hi, &dest_lo](CPU& cpu) {
            dest_hi = cpu.latch.W;
            dest_hi = cpu.latch.Z;
        }
    };
}
Instruction LD_a16_SP() {
    return Instruction {
        [](CPU& cpu) {cpu.latch.Z = cpu.fetch_byte();},
        [](CPU& cpu) {cpu.latch.W = cpu.fetch_byte();},
        [](CPU& cpu) {
            uint16_t addr = cpu.latch.combined();
            cpu.write_memory(addr, cpu.sp & 0xFF);
            cpu.latch.set(addr + 1);
        },
        [](CPU& cpu) {
            cpu.write_memory(cpu.latch.combined(), cpu.sp >> 8);
        },
        [](CPU& cpu) {}
    };
}
Instruction LD_SP_HL() {
    return Instruction {
        [](CPU& cpu) {cpu.sp = pair(cpu.H, cpu.L);},
        [](CPU& cpu) {}
    };
}
Instruction PUSH_rr(uint8_t& hi, uint8_t& lo) {
    return Instruction {
        [](CPU& cpu) {cpu.sp--;},
        [&hi](CPU& cpu) { 
            cpu.write_memory(cpu.sp, hi); 
            cpu.sp--;
        },
        [&lo](CPU& cpu) {
            cpu.write_memory(cpu.sp, lo);
            cpu.sp--;
        },
        [](CPU&) {}
    };
}
Instruction POP_rr(uint8_t& hi, uint8_t& lo) {
    return Instruction {
        [](CPU& cpu) {
            cpu.latch.Z = cpu.read_memory(cpu.sp);
            cpu.sp++;
        },
        [](CPU& cpu) {
            cpu.latch.W = cpu.read_memory(cpu.sp);
            cpu.sp++;
        },
        [&hi, &lo](CPU& cpu) {
            hi = cpu.latch.W;
            lo = cpu.latch.Z;
        }
    };
}

Instruction LD_HL_SPe() {
    return Instruction {
        [](CPU& cpu) {cpu.latch.Z = cpu.fetch_byte();},
        [](CPU& cpu) {},
        [](CPU& cpu) {
            bool half_carry = ((cpu.sp & 0xF) + (cpu.latch.Z & 0xF)) > 0xF;
            bool carry = ((cpu.sp & 0xFF) + cpu.latch.Z) > 0xFF;
            uint16_t result = cpu.sp + static_cast<int8_t>(cpu.latch.Z);

            cpu.set_flag(Flag::ZERO, 0);
            cpu.set_flag(Flag::NEGATIVE, 0);
            cpu.set_flag(Flag::HALF_CARRY, half_carry);
            cpu.set_flag(Flag::CARRY, carry);
            
            cpu.H = result >> 8;
        }
    };
}

//------------------- ARITHMETIC and LOGIC -------------------//
//generic 8-bit ALU op
using MathOp = void(*)(CPU& cpu, uint8_t reg);

Instruction ALU_Inst_r(MathOp op, const uint8_t& reg) {
    return Instruction {
        [&op, &reg](CPU& cpu) { op(cpu, reg); }
    };
}
Instruction ALU_Inst_m(MathOp op) {
    return Instruction { 
        [](CPU& cpu) {cpu.latch.Z = cpu.read_memory(pair(cpu.H, cpu.L));},
        [&op](CPU& cpu) {op(cpu, cpu.latch.Z);}
    };
}
Instruction ALU_Inst_m(MathOp op) {
    return Instruction { 
        [](CPU& cpu) {cpu.latch.Z = cpu.fetch_byte();},
        [&op](CPU& cpu) {op(cpu, cpu.latch.Z);}
    };
}

Instruction INC_r(uint8_t& reg) {
    return Instruction { 
        [&reg](CPU& cpu) {ALU::inc_8(cpu, reg); }
    };
}
Instruction INC_m() {
    return Instruction { 
        [](CPU& cpu) {cpu.latch.Z = cpu.read_memory(pair(cpu.H, cpu.L));},
        [](CPU& cpu) {ALU::inc_8(cpu, cpu.latch.Z);}
    };
}
Instruction INC_n() {
    return Instruction { 
        [](CPU& cpu) {cpu.latch.Z = cpu.fetch_byte();},
        [](CPU& cpu) {ALU::inc_8(cpu, cpu.latch.Z);}
    };
}
Instruction DEC_r(uint8_t& reg) {
    return Instruction { 
        [&reg](CPU& cpu) {ALU::dec_8(cpu, reg); }
    };
}
Instruction DEC_m() {
    return Instruction { 
        [](CPU& cpu) {cpu.latch.Z = cpu.read_memory(pair(cpu.H, cpu.L));},
        [](CPU& cpu) {ALU::dec_8(cpu, cpu.latch.Z);}
    };
}
Instruction DEC_n() {
    return Instruction { 
        [](CPU& cpu) {cpu.latch.Z = cpu.fetch_byte();},
        [](CPU& cpu) {ALU::dec_8(cpu, cpu.latch.Z);}
    };
}
Instruction CCF(){
    return Instruction {
        [](CPU& cpu) {
            cpu.set_flag(Flag::CARRY, !cpu.get_flag(Flag::CARRY));
            cpu.set_flag(Flag::NEGATIVE, 0);
            cpu.set_flag(Flag::HALF_CARRY, 0);
        }
    };
}

Instruction SCF(){
    return Instruction {
        [](CPU& cpu) {
            cpu.set_flag(Flag::CARRY, 1);
            cpu.set_flag(Flag::NEGATIVE, 0);
            cpu.set_flag(Flag::HALF_CARRY, 0);
        }
    };
}

Instruction DAA(){
    return Instruction{
        [](CPU& cpu) {/*TODO*/}
    };
}

Instruction CPL() {
    return Instruction {
        [](CPU& cpu) {
            cpu.A = ~cpu.A;
            cpu.set_flag(Flag::NEGATIVE, 1);
            cpu.set_flag(Flag::HALF_CARRY, 1);
        }
    };
}

//--------16-bit--------//
Instruction INC_16(Register16& rp) {
    return Instruction {
        [&](){ rp.set(rp.get() + 1); }
    };
}
Instruction INC_8(Register8& num, FlagRegister& fr) {
    return Instruction{ 
        [&](){
            uint8_t val = num.get();
            uint16_t result = val + 1;
            fr.set_flag(Flag::ZERO, (result&0xff) == 0);
            fr.set_flag(Flag::NEGATIVE, 0);
            fr.set_flag(Flag::HALF_CARRY, Arithmetic::half_carry_add_8(val, 1));
            num.set(result&0xff);
        }
    };
}
Instruction ADD_16(Register16& num1, const Register16& num2, FlagRegister& fr) {
    return Instruction{
        [&]() {
            uint16_t a = num1.get();
            uint16_t b = num2.get();
            int result = a + b;
            fr.set_flag(Flag::NEGATIVE, 0);
            fr.set_flag(Flag::HALF_CARRY, Arithmetic::half_carry_add_16(a, b));
            fr.set_flag(Flag::CARRY, result > 0xffff);
            num1.set(result);
        },
        1   
    };
} 
Instruction ADD_SP_e8(StackPointer& sp, const Immediate8& num2, FlagRegister& fr){
    return Instruction{
        [&]() {
            uint16_t a = sp.get();
            uint8_t ue = num2.get();    //"raw" unsigned num2
            int8_t e = static_cast<int8_t>(ue); // signed displacement
            uint16_t result = a + e; 

            bool half_carry = ((a & 0xF) + (ue & 0xF)) > 0xF;
            bool carry = ((a & 0xFF) + ue) > 0xFF;

            fr.set_flag(Flag::ZERO, 0);
            fr.set_flag(Flag::NEGATIVE, 0);
            fr.set_flag(Flag::HALF_CARRY, half_carry);
            fr.set_flag(Flag::CARRY, carry);

            sp.set(result);
        },
        2
    };
    //initial fetch (4) + operand fetch (4) + 16-bit add (4) + stack modification (4)
    //Total cycles: 16
}
Instruction SUB_8(Register8& num1, const Register8& num2, FlagRegister& fr) {
    return Instruction { 
        [&](){
            uint8_t a = num1.get();
            uint8_t b = num2.get();
            uint16_t result = a - b;
            fr.set_flag(Flag::ZERO, (result&0xff) == 0);
            fr.set_flag(Flag::NEGATIVE, 1);
            fr.set_flag(Flag::HALF_CARRY, Arithmetic::half_carry_sub_8(a, b));
            fr.set_flag(Flag::CARRY, b > a);
            num1.set(result & 0xff);
        }
    };
}
Instruction SBC_8(Register8& num1, const Register8& num2, FlagRegister& fr) {
    return Instruction { 
        [&](){
            uint8_t a = num1.get();
            uint8_t b = num2.get();
            bool c = fr.get_flag(Flag::CARRY);
            uint16_t result = a - b - c;
            fr.set_flag(Flag::ZERO, (result&0xff) == 0);
            fr.set_flag(Flag::NEGATIVE, 1);
            fr.set_flag(Flag::HALF_CARRY, ((a & 0xF) - (b & 0xF) - c) < 0);
            fr.set_flag(Flag::CARRY, b + c > a);
            num1.set(result & 0xff);
        }
    };
}
Instruction DEC_8(Register8& num, FlagRegister& fr){
    return Instruction{ 
        [&](){
            uint16_t result = num.get() - 1;
            fr.set_flag(Flag::ZERO, (result&0xff) == 0);
            fr.set_flag(Flag::NEGATIVE, 1);
            fr.set_flag(Flag::HALF_CARRY, Arithmetic::half_carry_sub_8(num.get(), 1));
            num.set(result&0xff);
        }
    };
}
Instruction DEC_16(Register16& rp) {
    return Instruction{
        [&](){
            rp.set(rp.get() - 1);
        }
    };
}     
//------------------LOGICAL------------------//
Instruction AND_8(Register8& num1, const Register8& num2, FlagRegister& fr) {
    return Instruction{
        [&](){
            num1.set(num1.get() & num2.get());
            fr = flag_state(num1.get() == 0, 0, 1, 0);
        }
    };
}
Instruction OR_8(Register8& num1, const Register8& num2, FlagRegister& fr) {
    return Instruction{
        [&](){
            num1.set(num1.get() | num2.get());
            fr = flag_state(num1.get() == 0, 0, 0, 0);
        }
    };
}

Instruction XOR_8(Register8& num1, const Register8& num2, FlagRegister& fr) {
    return Instruction{
        [&](){
            num1.set(num1.get() ^ num2.get());
            fr = flag_state(num1.get() == 0, 0, 0, 0);
        }
    };
}
Instruction CP_8(Register8& num1, const Register8& num2, FlagRegister& fr) {
    return Instruction { 
        [&](){
            uint8_t a = num1.get();
            uint8_t b = num2.get();
            uint16_t result = a - b;
            fr.set_flag(Flag::ZERO, (result&0xff) == 0);
            fr.set_flag(Flag::NEGATIVE, 1);
            fr.set_flag(Flag::HALF_CARRY, Arithmetic::half_carry_sub_8(a, b));
            fr.set_flag(Flag::CARRY, b > a);
            //no set
        } 
    };
}

//----------------------PREFIX OPS---------------------//
Instruction RLC(Register8& num, FlagRegister& fr){
    return Instruction{
        [&]() {
            bool carry = fr.get_flag(Flag::CARRY);
            uint8_t result = Arithmetic::rot_left_circ(num.get(), carry);
            fr.set(flag_state(!result, 0, 0, carry));
            num.set(result);
        }
    };
}
Instruction RRC(Register8& num, FlagRegister& fr){ 
    return Instruction{
        [&]() {
            bool carry = fr.get_flag(Flag::CARRY);
            uint8_t result = Arithmetic::rot_right_circ(num.get(), carry);
            fr.set(flag_state(!result, 0, 0, carry));
            num.set(result);
        }
    };
}
Instruction RL(Register8& num, FlagRegister& fr) {
    return Instruction{
        [&]() {
            bool carry = fr.get_flag(Flag::CARRY);
            uint8_t result = Arithmetic::rot_left(num.get(), carry);
            fr.set(flag_state(!result, 0, 0, carry));
            num.set(result);
        }
    };
}
Instruction RR(Register8& num, FlagRegister& fr) {
    return Instruction{
        [&]() {
            bool carry = fr.get_flag(Flag::CARRY);
            uint8_t result = Arithmetic::rot_right(num.get(), carry);
            fr.set(flag_state(!result, 0, 0, carry));
            num.set(result);
        }
    };
}
Instruction SLA(Register8& num, FlagRegister& fr) {
    return Instruction{
        [&]() {
            bool carry = fr.get_flag(Flag::CARRY);
            uint8_t result = Arithmetic::shift_left_arithmetic(num.get(), carry);
            fr.set(flag_state(!result, 0, 0, carry));
            num.set(result);
        }
    };
}
Instruction SRA(Register8& num, FlagRegister& fr){ 
    return Instruction{
        [&]() {
            bool carry = fr.get_flag(Flag::CARRY);
            uint8_t result = Arithmetic::shift_right_arithmetic(num.get(), carry);
            fr.set(flag_state(!result, 0, 0, carry));
            num.set(result);
        }
    };
}
Instruction SRL(Register8& num, FlagRegister& fr) {
    return Instruction{
        [&]() {
            bool carry = fr.get_flag(Flag::CARRY);
            uint8_t result = Arithmetic::shift_right_logical(num.get(), carry);
            fr.set(flag_state(!result, 0, 0, carry));
            num.set(result);
        }
    };
}
Instruction SWAP(Register8& num, FlagRegister& fr) {
    return Instruction{
        [&](){
            uint8_t x = Arithmetic::swap_nibs(num.get());
            fr.set(flag_state(x == 0, 0, 0, 0));
            num.set(x);
        }
    };
}
Instruction BIT(Register8& num, uint8_t bit, FlagRegister& fr) {
    return Instruction {
        [&](){
            bool bit_is_set = Arithmetic::bit_check(num.get(), bit);
            fr.set_flag(Flag::ZERO, bit_is_set);
            fr.set_flag(Flag::NEGATIVE, 0);
            fr.set_flag(Flag::HALF_CARRY, 1);
        }
    };
}
Instruction RES(Register8& num, uint8_t bit) {
    return Instruction{
        [&]() {
            uint8_t x = Arithmetic::bit_clear(num.get(), bit);
            num.set(x);
        }
    };
}
Instruction SET(Register8& num, uint8_t bit) {
    return Instruction{
        [&](){
            uint8_t x = Arithmetic::bit_set(num.get(), bit);
            num.set(x);
        }
    };
}

}   //Operation