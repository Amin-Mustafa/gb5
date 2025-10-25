#include "../include/Arithmetic.h"
#include "../include/Instruction.h"
#include "../include/Memory/MMU.h"
#include "../include/CPU.h"

using Arithmetic::pair;

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
    return ((z << 7) | (n << 6) | (h << 5) | (c << 4));
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
            cpu.F = flag_state(cpu.A == 0, 0, 1, 0);
        }
        void or_8(CPU& cpu, uint8_t num) {
            cpu.A = cpu.A | num;
            cpu.F = flag_state(cpu.A == 0, 0, 0, 0);
        }
        void xor_8(CPU& cpu, uint8_t num) {
            cpu.A = cpu.A ^ num;
            cpu.F = flag_state(cpu.A == 0, 0, 0, 0);
        }
        void decimal_adjust(CPU& cpu) {
            bool c_flag = cpu.get_flag(Flag::CARRY);
            bool h_flag = cpu.get_flag(Flag::HALF_CARRY);
            bool n_flag = cpu.get_flag(Flag::NEGATIVE);
            //after addition adjust if upper or lower nybble of A
            //out of BCD bounds
            if(!n_flag) {
                //...upper nybble
                if(c_flag || cpu.A > 0x99) {
                    cpu.A += 0x60;
                    c_flag = 1;
                }
                //...lower nybble
                if(h_flag || (cpu.A & 0x0f) > 0x09) {
                    cpu.A += 0x06;
                }
            }
            //after a subtraction, only adjust if lower nybble out of bounds
            else {
                if(c_flag) cpu.A -= 0x60;
                if(h_flag) cpu.A -= 0x06;
            }

            //update flag state
            cpu.F = flag_state(cpu.A == 0, n_flag, 0, c_flag);
        }
    } //ALU

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
            dest_lo = cpu.latch.Z;
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
Instruction LD_SP_n16() {
    return Instruction {
        [](CPU& cpu) {cpu.latch.Z = cpu.fetch_byte();},
        [](CPU& cpu) {cpu.latch.W = cpu.fetch_byte();},
        [](CPU& cpu) {cpu.sp = cpu.latch.combined(); }
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

Instruction POP_AF() {
    return Instruction {
        [](CPU& cpu) {
            cpu.latch.Z = cpu.read_memory(cpu.sp);
            cpu.sp++;
        },
        [](CPU& cpu) {
            cpu.latch.W = cpu.read_memory(cpu.sp);
            cpu.sp++;
        },
        [](CPU& cpu) {
            cpu.A = cpu.latch.W;
            cpu.F = cpu.latch.Z & 0xF0;
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
            cpu.L = result & 0xff;
        }
    };
}

//------------------- ARITHMETIC and LOGIC -------------------//
//generic 8-bit ALU op
Instruction ALU_Inst_r(MathOp op, const uint8_t& reg) {
    return Instruction {
        [op, &reg](CPU& cpu) { op(cpu, reg); }
    };
}
Instruction ALU_Inst_m(MathOp op) {
    return Instruction { 
        [](CPU& cpu) {cpu.latch.Z = cpu.read_memory(pair(cpu.H, cpu.L));},
        [op](CPU& cpu) {op(cpu, cpu.latch.Z);}
    };
}
Instruction ALU_Inst_n(MathOp op) {
    return Instruction { 
        [](CPU& cpu) {cpu.latch.Z = cpu.fetch_byte();},
        [op](CPU& cpu) {op(cpu, cpu.latch.Z);}
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
        [](CPU& cpu) {
            ALU::inc_8(cpu, cpu.latch.Z);
            cpu.write_memory(pair(cpu.H, cpu.L), cpu.latch.Z);
        },
        [](CPU& cpu) {}
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
        [](CPU& cpu) {
            ALU::dec_8(cpu, cpu.latch.Z);
            cpu.write_memory(pair(cpu.H, cpu.L), cpu.latch.Z);
        },
        [](CPU& cpu) {}
    };
}
Instruction CCF() {
    return Instruction {
        [](CPU& cpu) {
            cpu.set_flag(Flag::CARRY, !cpu.get_flag(Flag::CARRY));
            cpu.set_flag(Flag::NEGATIVE, 0);
            cpu.set_flag(Flag::HALF_CARRY, 0);
        }
    };
}

Instruction SCF() {
    return Instruction {
        [](CPU& cpu) {
            cpu.set_flag(Flag::CARRY, 1);
            cpu.set_flag(Flag::NEGATIVE, 0);
            cpu.set_flag(Flag::HALF_CARRY, 0);
        }
    };
}

Instruction DAA() {
    return Instruction{
        [](CPU& cpu) {ALU::decimal_adjust(cpu);}
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
Instruction INC_rr(uint8_t& reg1, uint8_t& reg2) {
    return Instruction {
        [&reg1, &reg2](CPU&) { 
            Pair pair{reg1, reg2};
            pair.set(pair.get() + 1);
        },
        [](CPU&) {}
    };
}
Instruction INC_SP() { 
    return Instruction {
        [](CPU& cpu) {cpu.sp++;},
        [](CPU& cpu) {}
    };
}
Instruction DEC_rr(uint8_t& reg1, uint8_t& reg2) {
    return Instruction {
        [&reg1, &reg2](CPU&) { 
            Pair pair{reg1, reg2};
            pair.set(pair.get() - 1);
        },
        [](CPU&) {}
    };
}
Instruction DEC_SP() { 
    return Instruction {
        [](CPU& cpu) {cpu.sp--;},
        [](CPU& cpu) {}
    };
}
Instruction ADD_HL_rr(uint8_t& reg1, uint8_t& reg2) {
    return Instruction{
        [&reg2](CPU& cpu) {
            uint16_t L_added = cpu.L + reg2;
            cpu.set_flag(Flag::NEGATIVE, 0);
            cpu.set_flag(Flag::HALF_CARRY, Arithmetic::half_carry_add_8(cpu.L, reg2));
            cpu.set_flag(Flag::CARRY, L_added > 0xff);
            cpu.L = L_added & 0xff;
        },
        [&reg1](CPU& cpu) {
            bool carry = cpu.get_flag(Flag::CARRY);
            uint16_t H_added = cpu.H + reg1 + carry;
            cpu.set_flag(Flag::NEGATIVE, 0);
            cpu.set_flag(Flag::HALF_CARRY, Arithmetic::half_carry_add_8(cpu.H, reg1, carry));
            cpu.set_flag(Flag::CARRY, H_added > 0xff);
            cpu.H = H_added;
        }
    };
}
Instruction ADD_HL_SP() {
    return Instruction{
        [](CPU& cpu) {
            uint8_t lo = cpu.sp & 0xff;
            uint16_t L_added = cpu.L + lo;
            cpu.set_flag(Flag::NEGATIVE, 0);
            cpu.set_flag(Flag::HALF_CARRY, Arithmetic::half_carry_add_8(cpu.L, lo));
            cpu.set_flag(Flag::CARRY, L_added > 0xff);
            cpu.L = L_added & 0xff;
        },
        [](CPU& cpu) {
            uint8_t hi = cpu.sp >> 8;
            bool carry = cpu.get_flag(Flag::CARRY);
            uint16_t H_added = cpu.H + hi + carry;
            cpu.set_flag(Flag::NEGATIVE, 0);
            cpu.set_flag(Flag::HALF_CARRY, Arithmetic::half_carry_add_8(cpu.H, hi, carry));
            cpu.set_flag(Flag::CARRY, H_added > 0xff);
            cpu.H = H_added;
        }
    };
}
Instruction ADD_SPe() {
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
            
            cpu.latch.set(result);
        },
        [](CPU& cpu) {cpu.sp = cpu.latch.combined();}
    };
}

//----------------------ROTATE, SHIFT, BIT----------------------//
using RotFunc = uint8_t(*)(uint8_t num, bool& carry); //rot/shift function
//-------Accumulator-------//
Instruction ROT_Inst_A(RotFunc func) {
    return Instruction {
        [func](CPU& cpu) {
            bool carry = cpu.get_flag(Flag::CARRY);
            cpu.A = func(cpu.A, carry);
            cpu.F = flag_state(0, 0, 0, carry);
        }
    };
}
//-------PREFIX ops--------//
Instruction PREFIX() {
    return {
        [](CPU& cpu) {
            cpu.prefix_mode();
        }
    };
}
Instruction ROT_Inst_r(RotFunc func, uint8_t& reg) {   
    return {
        [func, &reg](CPU& cpu) {
            bool carry = cpu.get_flag(Flag::CARRY);
            reg = func(reg, carry);
            cpu.F = flag_state(!reg, 0, 0, carry);
        }
    };
}
Instruction ROT_Inst_m(RotFunc func) {
    return {
        [](CPU& cpu) {cpu.latch.Z = cpu.read_memory(pair(cpu.H, cpu.L));},
        [func](CPU& cpu) {
            bool carry = cpu.get_flag(Flag::CARRY);
            uint8_t result = func(cpu.latch.Z, carry);
            cpu.write_memory(pair(cpu.H, cpu.L), result);
            cpu.F = flag_state(!result, 0, 0, carry);
        },
        [](CPU& cpu) {}
    };
}

Instruction BIT_r(uint8_t& reg, uint8_t bit) {
    return {
        [&reg, bit](CPU& cpu) {
            bool bit_is_set = Arithmetic::bit_check(reg, bit);
            cpu.set_flag(Flag::ZERO, !bit_is_set);
            cpu.set_flag(Flag::NEGATIVE, 0);
            cpu.set_flag(Flag::HALF_CARRY, 1);
        }
    };
}
Instruction BIT_m(uint8_t bit) {
    return {
        [](CPU& cpu) {cpu.latch.Z = cpu.read_memory(pair(cpu.H, cpu.L));},
        [bit](CPU& cpu) {
            bool bit_is_set = Arithmetic::bit_check(cpu.latch.Z, bit);
            cpu.set_flag(Flag::ZERO, !bit_is_set);
            cpu.set_flag(Flag::NEGATIVE, 0);
            cpu.set_flag(Flag::HALF_CARRY, 1);
        }
    };
}
Instruction SWAP_r(uint8_t& reg) {
    return {
        [&reg](CPU& cpu){
            reg = Arithmetic::swap_nibs(reg);
            cpu.F = flag_state(reg == 0, 0, 0, 0);
        }
    };
}
Instruction SWAP_m() {
    return {
        [](CPU& cpu) {cpu.latch.Z = cpu.read_memory(pair(cpu.H, cpu.L));},
        [](CPU& cpu){
            cpu.latch.Z = Arithmetic::swap_nibs(cpu.latch.Z);
            cpu.F = flag_state(cpu.latch.Z == 0, 0, 0, 0);
            cpu.write_memory(pair(cpu.H, cpu.L), cpu.latch.Z);
        },
        [](CPU& cpu) {}
    };
}
Instruction SET_r(uint8_t& reg, uint8_t bit) {
    return {
        [&reg, bit](CPU& cpu) {reg = Arithmetic::bit_set(reg, bit);}
    };
}
Instruction SET_m(uint8_t bit) {
    return {
        [](CPU& cpu) {cpu.latch.Z = cpu.read_memory(pair(cpu.H, cpu.L));},
        [bit](CPU& cpu) {
            cpu.latch.Z = Arithmetic::bit_set(cpu.latch.Z, bit);
            cpu.write_memory(pair(cpu.H, cpu.L), cpu.latch.Z);
        },
        [](CPU& cpu) {}
    };
}

Instruction RES_r(uint8_t& reg, uint8_t bit) {
    return {
        [&reg, bit](CPU& cpu) {reg = Arithmetic::bit_clear(reg, bit);}
    };
}

Instruction RES_m(uint8_t bit) {
    return {
        [](CPU& cpu) {cpu.latch.Z = cpu.read_memory(pair(cpu.H, cpu.L));},
        [bit](CPU& cpu) {
            cpu.latch.Z = Arithmetic::bit_clear(cpu.latch.Z, bit);
            cpu.write_memory(pair(cpu.H, cpu.L), cpu.latch.Z);
        },
        [](CPU& cpu) {}
    };
}

//----------------------CONTROL FLOW--------------------//
using ConditionCheck = bool(*)(const uint8_t& flags);
Instruction JP(ConditionCheck cc) {
    return Instruction {
        [](CPU& cpu) {cpu.latch.Z = cpu.fetch_byte();},
        [cc](CPU& cpu) {
            cpu.latch.W = cpu.fetch_byte();
            if(!cc(cpu.F)) cpu.skip_inst();
        },
        [](CPU& cpu) {cpu.pc = cpu.latch.combined() - 1;},
        [](CPU& cpu) {}
    };
}
Instruction JPHL(){
    return Instruction{
        [](CPU& cpu) {cpu.pc = pair(cpu.H, cpu.L) - 1;}
    };
}
Instruction JR(ConditionCheck cc) {
    return Instruction {
        [cc](CPU& cpu) {
            cpu.latch.Z = cpu.fetch_byte();
            if(!cc(cpu.F)) cpu.skip_inst();
        },
        [](CPU& cpu) {
            int8_t offset = static_cast<int8_t>(cpu.latch.Z);
            cpu.latch.set(cpu.pc + offset);
            cpu.pc = cpu.latch.combined();
        },
        [](CPU& cpu) {}
    };
}
Instruction CALL(ConditionCheck cc){
    return Instruction {
        [](CPU& cpu) {cpu.latch.Z = cpu.fetch_byte();},
        [cc](CPU& cpu) {
            cpu.latch.W = cpu.fetch_byte();
            if(!cc(cpu.F)) cpu.skip_inst();
        },
        [](CPU& cpu) {cpu.sp--;},
        [](CPU& cpu) {
            cpu.write_memory(cpu.sp, (cpu.pc+1) >> 8);
            cpu.sp--;
        },
        [](CPU& cpu) {
            cpu.write_memory(cpu.sp, (cpu.pc+1) & 0xff);
            cpu.pc = cpu.latch.combined() - 1;
        },
        [](CPU&){}
    };
}
Instruction RET(){ 
    //absolute return takes 4 cycles,
    //conditional return takes 5 cycles if condition is true
    return Instruction{
        [](CPU& cpu) {
            cpu.latch.Z = cpu.read_memory(cpu.sp);
            cpu.sp++;
        },
        [](CPU& cpu) {
            cpu.latch.W = cpu.read_memory(cpu.sp);
            cpu.sp++;
        },
        [](CPU& cpu) {
            cpu.pc = cpu.latch.combined() - 1;
        },
        [](CPU& cpu) {}
    };
}
Instruction RET_IF(ConditionCheck cc) {
    //unlike conditional CALL, JP, JR, conditional RET has 
    //a dedicated cycle just for condition check
    return Instruction {
        [cc](CPU& cpu) {
            if(!cc(cpu.F)) cpu.skip_inst();
        },
        [](CPU& cpu) {
            cpu.latch.Z = cpu.read_memory(cpu.sp);
            cpu.sp++;
        },
        [](CPU& cpu) {
            cpu.latch.W = cpu.read_memory(cpu.sp);
            cpu.sp++;
        },
        [](CPU& cpu) {
            cpu.pc = cpu.latch.combined() - 1;
        },
        [](CPU& cpu) {}
    };
}

Instruction RETI() {
    return Instruction{
        [](CPU& cpu) {
            cpu.latch.Z = cpu.read_memory(cpu.sp);
            cpu.sp++;
        },
        [](CPU& cpu) {
            cpu.latch.W = cpu.read_memory(cpu.sp);
            cpu.sp++;
        },
        [](CPU& cpu) {
            cpu.pc = cpu.latch.combined() - 1;
            cpu.IME = true;
        },
        [](CPU& cpu) {}
    };
}

Instruction RST(uint8_t addr) {
    //CALL to fixed 1-byte address
    return Instruction {
        [](CPU& cpu) {cpu.sp--;},
        [](CPU& cpu) {
            cpu.write_memory(cpu.sp, (cpu.pc + 1) >> 8);
            cpu.sp--;
        },
        [addr](CPU& cpu) {
            cpu.write_memory(cpu.sp, (cpu.pc + 1) & 0xff);
            cpu.pc = addr - 1;
        },
        [](CPU& cpu) {}
    };
}

//--------------------MISC-------------------//
Instruction DI() {
    return Instruction {
        [](CPU& cpu) {cpu.IME = false;}
    };
}
Instruction EI() {
    return Instruction {
        [](CPU& cpu) {
            cpu.int_enable_pending = true;
        }
    };
}

Instruction HALT() {
    return Instruction {
        [](CPU& cpu) {cpu.halt();}
    };
}


}   //Operation