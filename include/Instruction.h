#ifndef INSTRUCTION_H
#define INSTRUCTION_H

#include <string>
#include <iostream>
#include <functional>
#include <cstdint>
#include <algorithm>

class CPU;

namespace Operation {
    namespace ALU {
        //primitive arithmetic and logic micro ops
        void add_8(CPU& cpu, uint8_t num);
        void adc_8(CPU& cpu, uint8_t num);
        void sub_8(CPU& cpu, uint8_t num);
        void sbc_8(CPU& cpu, uint8_t num);
        void cp_8(CPU& cpu, uint8_t num);
        void inc_8(CPU& cpu, uint8_t& num);
        void dec_8(CPU& cpu, uint8_t& num);
        void and_8(CPU& cpu, uint8_t num);
        void or_8(CPU& cpu, uint8_t num);
        void xor_8(CPU& cpu, uint8_t num);
    }

//------------------------ LOADS ------------------------//
//8-bit
void NOP(CPU& cpu);
void LD_r_r(CPU& cpu, uint8_t& dest, const uint8_t& src);
void LD_r_n(CPU& cpu, uint8_t& dest);
void LD_r_m(CPU& cpu, uint8_t& dest, uint8_t& src_hi, uint8_t& src_lo);
void LD_m_r(CPU& cpu, uint8_t& dest_hi, uint8_t& dest_lo, uint8_t& src);
void LD_m_n(CPU& cpu, uint8_t& dest_hi, uint8_t& dest_lo);
void LD_A_a16(CPU& cpu);
void LD_a16_A(CPU& cpu);
void LDH_A_C(CPU& cpu);
void LDH_C_A(CPU& cpu);
void LDH_A_n(CPU& cpu);
void LDH_n_A(CPU& cpu);
void LD_A_HLdec(CPU& cpu);
void LD_HLdec_A(CPU& cpu);
void LD_A_HLinc(CPU& cpu);
void LD_HLinc_A(CPU& cpu);
//16-bit
void LD_rr_n16(CPU& cpu, uint8_t& dest_hi, uint8_t& dest_lo);
void LD_a16_SP(CPU& cpu);
void LD_SP_HL(CPU& cpu);
void LD_SP_n16(CPU& cpu);
void PUSH_rr(CPU& cpu, uint8_t& hi, uint8_t& lo);
void POP_rr(CPU& cpu, uint8_t& hi, uint8_t& lo);
void POP_AF(CPU& cpu);
void LD_HL_SPe(CPU& cpu);

//------------------- ARITHMETIC -------------------//
//8-bit
using MathOp = void(*)(CPU& cpu, uint8_t reg);
void ALU_Inst_r(CPU& cpu, MathOp op, const uint8_t& reg);
void ALU_Inst_m(CPU& cpu, MathOp op);
void ALU_Inst_n(CPU& cpu, MathOp op);
void INC_r(CPU& cpu, uint8_t& reg);
void INC_m(CPU& cpu);
void INC_n(CPU& cpu);
void DEC_r(CPU& cpu, uint8_t& reg);
void DEC_m(CPU& cpu);
void DEC_n(CPU& cpu);
void CCF(CPU& cpu);
void SCF(CPU& cpu);
void DAA(CPU& cpu);
void CPL(CPU& cpu);
//16-bit
void INC_rr(CPU& cpu, uint8_t& reg1, uint8_t& reg2);
void INC_SP(CPU& cpu);
void DEC_rr(CPU& cpu, uint8_t& reg1, uint8_t& reg2);
void DEC_SP(CPU& cpu);
void ADD_HL_rr(CPU& cpu, uint8_t& reg1, uint8_t& reg2);
void ADD_HL_SP(CPU& cpu);
void ADD_SPe(CPU& cpu);

//----------------------ROTATE, SHIFT, BIT----------------------//
using RotFunc = uint8_t(*)(uint8_t num, bool& carry); //rot/shift function
void ROT_Inst_A(CPU& cpu, RotFunc func);
//---------PREFIX ops---------//
void PREFIX(CPU& cpu);
void ROT_Inst_r(CPU& cpu, RotFunc func, uint8_t& reg);
void ROT_Inst_m(CPU& cpu, RotFunc func);
void BIT_r(CPU& cpu, uint8_t& reg, uint8_t bit);
void BIT_m(CPU& cpu, uint8_t bit);
void SWAP_r(CPU& cpu, uint8_t& reg);
void SWAP_m(CPU& cpu);
void SET_r(CPU& cpu, uint8_t& reg, uint8_t bit);
void SET_m(CPU& cpu, uint8_t bit);
void RES_r(CPU& cpu, uint8_t& reg, uint8_t bit);
void RES_m(CPU& cpu, uint8_t bit);

//----------------------CONTROL FLOW--------------------//
using ConditionCheck = bool(*)(const uint8_t& flags);
void JP(CPU& cpu, ConditionCheck cc);
void JPHL(CPU& cpu);
void JR(CPU& cpu, ConditionCheck cc);
void CALL(CPU& cpu, ConditionCheck cc);
void RET(CPU& cpu);
void RET_IF(CPU& cpu, ConditionCheck cc);
void RETI(CPU& cpu);
void RST(CPU& cpu, uint8_t addr);
void ISR(CPU& cpu, uint8_t addr);

//--------------------------MISC------------------------//
void DI(CPU& cpu);
void EI(CPU& cpu);
void HALT(CPU& cpu);
}   //Operation

#endif