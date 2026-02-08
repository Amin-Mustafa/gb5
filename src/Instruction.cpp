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
            uint8_t correction = 0;
            bool n = cpu.get_flag(Flag::NEGATIVE);
            bool h = cpu.get_flag(Flag::HALF_CARRY);
            bool c = cpu.get_flag(Flag::CARRY);

            if (h || (!n && (cpu.A & 0x0F) > 9)) {
                correction |= 0x06;
            }
            if (c || (!n && cpu.A > 0x99)) {
                correction |= 0x60;
                c = true;
            }
            if (n) {
                cpu.A -= correction; 
            } else {
                cpu.A += correction; 
            }
            cpu.set_flag(Flag::ZERO, cpu.A == 0);
            cpu.set_flag(Flag::HALF_CARRY, false); //h always cleared
            cpu.set_flag(Flag::CARRY, c);          //c updated
        }
    } //ALU

void NOP(CPU& cpu) {}
//----------------------LOADS-----------------------//
//------8-bit------//
void LD_r_r(CPU& cpu, uint8_t& dest, const uint8_t& src) {
    dest = src;
}
void LD_r_n(CPU& cpu, uint8_t& dest) {
    uint8_t src = cpu.fetch_byte();
    dest = src;
}
void LD_r_m(CPU& cpu, uint8_t& dest, uint8_t& src_hi, uint8_t& src_lo) {
    uint8_t src = cpu.read_memory(pair(src_hi, src_lo));
    dest = src;
}
void LD_m_r(CPU& cpu, uint8_t& dest_hi, uint8_t& dest_lo, uint8_t& src) {
    cpu.write_memory(pair(dest_hi, dest_lo), src);
}
void LD_m_n(CPU& cpu, uint8_t& dest_hi, uint8_t& dest_lo) {
        uint8_t src = cpu.fetch_byte();
        cpu.write_memory(pair(dest_hi, dest_lo), src);
}
void LD_A_a16(CPU& cpu) {
    uint8_t lo = cpu.fetch_byte();
    uint8_t hi = cpu.fetch_byte();
    uint8_t src = cpu.read_memory(pair(hi, lo));
    cpu.A = src;
}
void LD_a16_A(CPU& cpu) {
    uint8_t lo = cpu.fetch_byte();
    uint8_t hi = cpu.fetch_byte();
    cpu.write_memory(pair(hi, lo), cpu.A);
}
void LDH_A_C(CPU& cpu) {
    uint8_t src = cpu.read_memory(pair(0xFF, cpu.C));
    cpu.A = src;
}
void LDH_C_A(CPU& cpu){
    cpu.write_memory(pair(0xFF, cpu.C), cpu.A);
}
void LDH_A_n(CPU& cpu){
    uint8_t lo = cpu.fetch_byte();
    uint8_t src = cpu.read_memory(pair(0xFF, lo));
    cpu.A = src;
}
void LDH_n_A(CPU& cpu){
    uint8_t lo = cpu.fetch_byte();
    cpu.write_memory(pair(0xFF, lo), cpu.A);
}
void LD_A_HLdec(CPU& cpu) {
    Pair hl{cpu.H, cpu.L};
    uint8_t src = cpu.read_memory(hl.get());
    hl.set(hl.get()-1);
    cpu.A = src;
}
void LD_HLdec_A(CPU& cpu) {
    Pair hl{cpu.H, cpu.L};
    cpu.write_memory(hl.get(), cpu.A);
    hl.set(hl.get()-1);
}
void LD_A_HLinc(CPU& cpu) {
    Pair HL{cpu.H, cpu.L};
    uint8_t src = cpu.read_memory(HL.get());
    HL.set(HL.get()+1);
    cpu.A = src;
}
void LD_HLinc_A(CPU& cpu) {
    Pair HL{cpu.H, cpu.L};
    cpu.write_memory(HL.get(), cpu.A);
    HL.set(HL.get() + 1);
}

//--------16-bit--------//
void LD_rr_n16(CPU& cpu, uint8_t& dest_hi, uint8_t& dest_lo) {
    uint8_t lo = cpu.fetch_byte();
    uint8_t hi = cpu.fetch_byte();

    dest_hi = hi;
    dest_lo = lo;
}
void LD_a16_SP(CPU& cpu) {
    uint8_t lo = cpu.fetch_byte();
    uint8_t hi = cpu.fetch_byte();
    Pair addr{hi, lo};
    cpu.write_memory(addr.get(), cpu.sp & 0xFF);
    addr.set(addr.get() + 1);
    cpu.write_memory(addr.get(), cpu.sp >> 8);

}
void LD_SP_HL(CPU& cpu) {
    uint16_t addr = pair(cpu.H, cpu.L);
    cpu.sp = addr;
    cpu.idle_m_cycle();
}
void LD_SP_n16(CPU& cpu) {
    uint8_t lo = cpu.fetch_byte();
    uint8_t hi = cpu.fetch_byte();
    cpu.sp = pair(hi, lo);
}
void PUSH_rr(CPU& cpu, uint8_t& hi, uint8_t& lo) {
    cpu.sp--;
    cpu.write_memory(cpu.sp, hi);
    cpu.sp--;
    cpu.write_memory(cpu.sp, lo);

    cpu.idle_m_cycle();
}
void POP_rr(CPU& cpu, uint8_t& hi, uint8_t& lo) {
    lo = cpu.read_memory(cpu.sp);
    cpu.sp++;
    hi = cpu.read_memory(cpu.sp);
    cpu.sp++;
}

void POP_AF(CPU& cpu) {
    cpu.F = cpu.read_memory(cpu.sp) & 0xF0;
    cpu.sp++;
    cpu.A = cpu.read_memory(cpu.sp);
    cpu.sp++;
}

void LD_HL_SPe(CPU& cpu) {
    uint8_t offset = cpu.fetch_byte();
    cpu.idle_m_cycle(); //dummy cycle
    bool half_carry = ((cpu.sp & 0xF) + (offset & 0xF)) > 0xF;
    bool carry = ((cpu.sp & 0xFF) + offset) > 0xFF;
    uint16_t result = cpu.sp + static_cast<int8_t>(offset);

    cpu.set_flag(Flag::ZERO, 0);
    cpu.set_flag(Flag::NEGATIVE, 0);
    cpu.set_flag(Flag::HALF_CARRY, half_carry);
    cpu.set_flag(Flag::CARRY, carry);
    
    cpu.H = result >> 8;
    cpu.L = result & 0xff;
}

//------------------- ARITHMETIC and LOGIC -------------------//
//generic 8-bit ALU op
void ALU_Inst_r(CPU& cpu, MathOp op, const uint8_t& reg) {
    op(cpu, reg);
}
void ALU_Inst_m(CPU& cpu, MathOp op) {
    uint8_t arg = cpu.read_memory(pair(cpu.H, cpu.L));
    op(cpu, arg);
}
void ALU_Inst_n(CPU& cpu, MathOp op) {
    uint8_t arg = cpu.fetch_byte();
    op(cpu, arg);
}

void INC_r(CPU& cpu, uint8_t& reg) {
    ALU::inc_8(cpu, reg);
}
void INC_m(CPU& cpu) {
    uint8_t arg = cpu.read_memory(pair(cpu.H, cpu.L));
    ALU::inc_8(cpu, arg);
    cpu.write_memory(pair(cpu.H, cpu.L), arg);
}
void DEC_r(CPU& cpu, uint8_t& reg) {
    ALU::dec_8(cpu, reg);
}
void DEC_m(CPU& cpu) {
    uint8_t arg = cpu.read_memory(pair(cpu.H, cpu.L));
    ALU::dec_8(cpu, arg);
    cpu.write_memory(pair(cpu.H, cpu.L), arg);
}
void CCF(CPU& cpu) {
    cpu.set_flag(Flag::CARRY, !cpu.get_flag(Flag::CARRY));
    cpu.set_flag(Flag::NEGATIVE, 0);
    cpu.set_flag(Flag::HALF_CARRY, 0);
}

void SCF(CPU& cpu) {
    cpu.set_flag(Flag::CARRY, 1);
    cpu.set_flag(Flag::NEGATIVE, 0);
    cpu.set_flag(Flag::HALF_CARRY, 0);
}

void DAA(CPU& cpu) {
    ALU::decimal_adjust(cpu);
}

void CPL(CPU& cpu) {
    cpu.A = ~cpu.A;
    cpu.set_flag(Flag::NEGATIVE, 1);
    cpu.set_flag(Flag::HALF_CARRY, 1);
}

//--------16-bit--------//
void INC_rr(CPU& cpu, uint8_t& reg1, uint8_t& reg2) {
    Pair pair{reg1, reg2};
    pair.set(pair.get() + 1);
    cpu.idle_m_cycle();
}
void INC_SP(CPU& cpu) { 
    cpu.sp++;
    cpu.idle_m_cycle();
}
void DEC_rr(CPU& cpu, uint8_t& reg1, uint8_t& reg2) {
    Pair pair{reg1, reg2};
    pair.set(pair.get() - 1);
    cpu.idle_m_cycle();
}
void DEC_SP(CPU& cpu) { 
    cpu.sp--;
    cpu.idle_m_cycle();
}
void ADD_HL_rr(CPU& cpu, uint8_t& reg1, uint8_t& reg2) {
    uint16_t L_addend = cpu.L + reg2;
    cpu.set_flag(Flag::NEGATIVE, 0);
    cpu.set_flag(Flag::HALF_CARRY, Arithmetic::half_carry_add_8(cpu.L, reg2));
    cpu.set_flag(Flag::CARRY, L_addend > 0xff);
    cpu.L = L_addend & 0xff;

    bool carry = cpu.get_flag(Flag::CARRY);
    uint16_t H_addend = cpu.H + reg1 + carry;
    cpu.set_flag(Flag::NEGATIVE, 0);
    cpu.set_flag(Flag::HALF_CARRY, Arithmetic::half_carry_add_8(cpu.H, reg1, carry));
    cpu.set_flag(Flag::CARRY, H_addend > 0xff);
    cpu.H = H_addend;

    cpu.idle_m_cycle();
}
void ADD_HL_SP(CPU& cpu) {
    uint8_t lo = cpu.sp & 0xff;
    uint16_t L_addend = cpu.L + lo;

    cpu.set_flag(Flag::NEGATIVE, 0);
    cpu.set_flag(Flag::HALF_CARRY, Arithmetic::half_carry_add_8(cpu.L, lo));
    cpu.set_flag(Flag::CARRY, L_addend > 0xff);
    cpu.L = L_addend & 0xff;
    uint8_t hi = cpu.sp >> 8;

    bool carry = cpu.get_flag(Flag::CARRY);
    uint16_t H_addend = cpu.H + hi + carry;
    cpu.set_flag(Flag::NEGATIVE, 0);
    cpu.set_flag(Flag::HALF_CARRY, Arithmetic::half_carry_add_8(cpu.H, hi, carry));
    cpu.set_flag(Flag::CARRY, H_addend > 0xff);
    cpu.H = H_addend;

    cpu.idle_m_cycle();
}
void ADD_SPe(CPU& cpu) {
    uint8_t offset = cpu.fetch_byte();

    cpu.idle_m_cycle();

    bool half_carry = ((cpu.sp & 0xF) + (offset & 0xF)) > 0xF;
    bool carry = ((cpu.sp & 0xFF) + offset) > 0xFF;
    uint16_t result = cpu.sp + static_cast<int8_t>(offset);

    cpu.set_flag(Flag::ZERO, 0);
    cpu.set_flag(Flag::NEGATIVE, 0);
    cpu.set_flag(Flag::HALF_CARRY, half_carry);
    cpu.set_flag(Flag::CARRY, carry);

    cpu.sp = result;
    cpu.idle_m_cycle();
}

//----------------------ROTATE, SHIFT, BIT----------------------//
using RotFunc = uint8_t(*)(uint8_t num, bool& carry); //rot/shift function
//-------Accumulator-------//
void ROT_Inst_A(CPU& cpu, RotFunc func) {
    bool carry = cpu.get_flag(Flag::CARRY);
    cpu.A = func(cpu.A, carry);
    cpu.F = flag_state(0, 0, 0, carry);
}
//-------PREFIX ops--------//
void PREFIX(CPU& cpu) {
    cpu.prefix_mode();
}
void ROT_Inst_r(CPU& cpu, RotFunc func, uint8_t& reg) {   
    bool carry = cpu.get_flag(Flag::CARRY);
    reg = func(reg, carry);
    cpu.F = flag_state(!reg, 0, 0, carry);
}
void ROT_Inst_m(CPU& cpu, RotFunc func) {
    uint8_t arg = cpu.read_memory(pair(cpu.H, cpu.L));
    bool carry = cpu.get_flag(Flag::CARRY);
    uint8_t result = func(arg, carry);
    cpu.write_memory(pair(cpu.H, cpu.L), result);
    cpu.F = flag_state(!result, 0, 0, carry);
}

void BIT_r(CPU& cpu, uint8_t& reg, uint8_t bit) {
     bool bit_is_set = Arithmetic::bit_check(reg, bit);
    cpu.set_flag(Flag::ZERO, !bit_is_set);
    cpu.set_flag(Flag::NEGATIVE, 0);
    cpu.set_flag(Flag::HALF_CARRY, 1);
}
void BIT_m(CPU& cpu, uint8_t bit) {
    uint8_t arg = cpu.read_memory(pair(cpu.H, cpu.L));
    bool bit_is_set = Arithmetic::bit_check(arg, bit);
    cpu.set_flag(Flag::ZERO, !bit_is_set);
    cpu.set_flag(Flag::NEGATIVE, 0);
    cpu.set_flag(Flag::HALF_CARRY, 1);
}
void SWAP_r(CPU& cpu, uint8_t& reg) {
    reg = Arithmetic::swap_nibs(reg);
    cpu.F = flag_state(reg == 0, 0, 0, 0);
}
void SWAP_m(CPU& cpu) {
    uint8_t arg = cpu.read_memory(pair(cpu.H, cpu.L));
    arg = Arithmetic::swap_nibs(arg);
    cpu.F = flag_state(arg == 0, 0, 0, 0);
    cpu.write_memory(pair(cpu.H, cpu.L), arg);
}
void SET_r(CPU& cpu, uint8_t& reg, uint8_t bit) {   
    reg = Arithmetic::bit_set(reg, bit);
}
void SET_m(CPU& cpu, uint8_t bit) {
    uint8_t arg = cpu.read_memory(pair(cpu.H, cpu.L));
    arg = Arithmetic::bit_set(arg, bit);
    cpu.write_memory(pair(cpu.H, cpu.L), arg);
}

void RES_r(CPU& cpu, uint8_t& reg, uint8_t bit) {
    reg = Arithmetic::bit_clear(reg, bit);
}

void RES_m(CPU& cpu, uint8_t bit) {
    uint8_t arg = cpu.read_memory(pair(cpu.H, cpu.L));
    arg = Arithmetic::bit_clear(arg, bit);
    cpu.write_memory(pair(cpu.H, cpu.L), arg);
}

//----------------------CONTROL FLOW--------------------//
using ConditionCheck = bool(*)(const uint8_t& flags);
void JP(CPU& cpu, ConditionCheck cc) {
    uint8_t addr_lo = cpu.fetch_byte();
    uint8_t addr_hi = cpu.fetch_byte();

    if(!cc(cpu.F)) {
        return;
    }

    cpu.pc = pair(addr_hi, addr_lo);
    cpu.idle_m_cycle();
}
void JPHL(CPU& cpu){
    cpu.pc = pair(cpu.H, cpu.L);
}
void JR(CPU& cpu, ConditionCheck cc) {
    uint8_t byte = cpu.fetch_byte();

    if(!cc(cpu.F)) {
        return;
    }

    int8_t offset = static_cast<int8_t>(byte);
    cpu.pc = cpu.pc + offset;
    cpu.idle_m_cycle();
}
void CALL(CPU& cpu, ConditionCheck cc){
    uint8_t addr_lo = cpu.fetch_byte();
    uint8_t addr_hi = cpu.fetch_byte();

    if(!cc(cpu.F)) return;

    cpu.sp--;
    cpu.write_memory(cpu.sp, cpu.pc >> 8);
    cpu.sp--;
    cpu.write_memory(cpu.sp, cpu.pc & 0xff);
    cpu.pc = pair(addr_hi, addr_lo);

    cpu.idle_m_cycle();
}
void RET(CPU& cpu){ 
    //absolute return takes 4 cycles,
    //conditional return takes 5 cycles if condition is true
    uint8_t addr_lo = cpu.read_memory(cpu.sp);
    cpu.sp++;
    uint8_t addr_hi = cpu.read_memory(cpu.sp);
    cpu.sp++;
    cpu.pc = pair(addr_hi, addr_lo);
    cpu.idle_m_cycle();
}
void RET_IF(CPU& cpu, ConditionCheck cc) {
    //unlike conditional CALL, JP, JR, conditional RET has 
    //a dedicated cycle just for condition check
    cpu.idle_m_cycle();
    if(!cc(cpu.F)) {
        return;
    }

    uint8_t addr_lo = cpu.read_memory(cpu.sp);
    cpu.sp++;
    uint8_t addr_hi = cpu.read_memory(cpu.sp);
    cpu.sp++;
    cpu.pc = pair(addr_hi, addr_lo);
    cpu.idle_m_cycle();
}

void RETI(CPU& cpu) {
    uint8_t addr_lo = cpu.read_memory(cpu.sp);
    cpu.sp++;
    uint8_t addr_hi = cpu.read_memory(cpu.sp);
    cpu.sp++;
    cpu.pc = pair(addr_hi, addr_lo);
    cpu.IME = true;
    cpu.idle_m_cycle();
}

void RST(CPU& cpu, uint8_t addr) {
    //CALL to fixed 1-byte address
    cpu.sp--;
    cpu.write_memory(cpu.sp, cpu.pc >> 8);
    cpu.sp--;
    cpu.write_memory(cpu.sp, cpu.pc & 0xff);
    cpu.pc = addr;
    cpu.idle_m_cycle();
}

//--------------------MISC-------------------//
void DI(CPU& cpu) {
    cpu.IME = false;
}
void EI(CPU& cpu) {
    cpu.schedule_ei();
}

void  HALT(CPU& cpu) {
    cpu.halt();
}


}   //Operation