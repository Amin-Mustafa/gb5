#ifndef INSTRUCTION_H
#define INSTRUCTION_H

#include <string>
#include <iostream>
#include <functional>
#include <cstdint>

class CPU;

class Instruction {
public:
    typedef std::function<void(CPU&)> MicroOp;

    Instruction()   
        :ops{} {}

    Instruction(std::initializer_list<MicroOp> sub_ops)
        :ops{sub_ops} {}

    int length() const {return ops.size();}
    void execute_subop(CPU& cpu, int index) { ops[index](cpu); }
private:
    //each instruction is divided into atomic sub-operations
    //that each take 1 cpu m-cycle
    std::vector<MicroOp> ops; 
};

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

Instruction NOP();

//------------------------ LOADS ------------------------//
//8-bit
Instruction LD_r_r(uint8_t& dest, const uint8_t& src);
Instruction LD_r_n(uint8_t& dest);
Instruction LD_r_m(uint8_t& dest, uint8_t& src_hi, uint8_t& src_lo);
Instruction LD_m_r(uint8_t& dest_hi, uint8_t& dest_lo, uint8_t& src);
Instruction LD_m_n(uint8_t& dest_hi, uint8_t& dest_lo);
Instruction LD_A_a16();
Instruction LD_a16_A();
Instruction LDH_A_C();
Instruction LDH_C_A();
Instruction LDH_A_n();
Instruction LDH_n_A();
Instruction LD_A_HLdec();
Instruction LD_HLdec_A();
Instruction LD_A_HLinc();
Instruction LD_HLinc_A();
//16-bit
Instruction LD_rr_n16(uint8_t& dest_hi, uint8_t& dest_lo);
Instruction LD_a16_SP();
Instruction LD_SP_HL();
Instruction LD_SP_n16();
Instruction PUSH_rr(uint8_t& hi, uint8_t& lo);
Instruction POP_rr(uint8_t& hi, uint8_t& lo);
Instruction LD_HL_SPe();

//------------------- ARITHMETIC -------------------//
//8-bit
using MathOp = void(*)(CPU& cpu, uint8_t reg);
Instruction ALU_Inst_r(MathOp op, const uint8_t& reg);
Instruction ALU_Inst_m(MathOp op);
Instruction ALU_Inst_n(MathOp op);
Instruction INC_r(uint8_t& reg);
Instruction INC_m();
Instruction INC_n();
Instruction DEC_r(uint8_t& reg);
Instruction DEC_m();
Instruction DEC_n();
Instruction CCF();
Instruction SCF();
Instruction DAA();
Instruction CPL();
//16-bit
Instruction INC_rr(uint8_t& reg1, uint8_t& reg2);
Instruction INC_SP();
Instruction DEC_rr(uint8_t& reg1, uint8_t& reg2);
Instruction DEC_SP();
Instruction ADD_HL_rr(uint8_t& reg1, uint8_t& reg2);
Instruction ADD_HL_SP();
Instruction ADD_SPe();

//----------------------ROTATE, SHIFT, BIT----------------------//
using RotFunc = uint8_t(*)(uint8_t num, bool& carry); //rot/shift function
Instruction ROT_Inst_A(RotFunc func);
//---------PREFIX ops---------//
Instruction PREFIX();
Instruction PREFIX_Inst_r(Instruction::MicroOp op);
Instruction PREFIX_Inst_m(Instruction::MicroOp op);
Instruction::MicroOp ROT_r(RotFunc func, uint8_t& reg);
Instruction::MicroOp ROT_m(RotFunc func);
Instruction::MicroOp BIT_r(uint8_t& reg, uint8_t bit);
Instruction::MicroOp BIT_m(uint8_t bit);
Instruction::MicroOp SWAP_r(uint8_t& reg);
Instruction::MicroOp SWAP_m();
Instruction::MicroOp SET_r(uint8_t& reg, uint8_t bit);
Instruction::MicroOp SET_m(uint8_t bit);
Instruction::MicroOp RES_r(uint8_t& reg, uint8_t bit);
Instruction::MicroOp RES_m(uint8_t bit);

//----------------------CONTROL FLOW--------------------//
using ConditionCheck = bool(*)(const uint8_t& flags);
Instruction JP(ConditionCheck cc);
Instruction JPHL();
Instruction JR(ConditionCheck cc);
Instruction CALL(ConditionCheck cc);
Instruction RET();
Instruction RET_IF(ConditionCheck cc);
Instruction RETI();
Instruction RST(uint8_t addr);

//--------------------------MISC------------------------//
Instruction DI();
Instruction EI();
}   //Operation

#endif