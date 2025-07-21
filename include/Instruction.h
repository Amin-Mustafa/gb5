#ifndef INSTRUCTION_H
#define INSTRUCTION_H

#include <string>
#include <iostream>
#include <functional>
#include <cstdint>

class MMU;
class FlagRegister;

class Instruction {
public:
    typedef std::function<void()> Opfn;

    Instruction()
        :op{[](){}}, len{1}, num_ticks{4} {}
    Instruction(Opfn operation, int length, int ticks)
        :op{std::move(operation)}, len{length}, num_ticks{ticks} {}
    
    void execute() { op(); }
    int length() const {return len;}
    int cycles() const {return num_ticks;}
private:
    Opfn op;
    int len;
    int num_ticks;
};

namespace Operation {
//------------------------ LOADS ------------------------//
Instruction LD_8_reg(uint8_t& dest, uint8_t& src);
Instruction LD_8_mem_r(MMU& mmu, uint16_t addr, uint8_t& src);
Instruction LD_8_r_mem(uint8_t& dest, MMU& mmu, uint16_t addr);
Instruction LD_8_imm(uint8_t& dest, uint8_t src);
Instruction LD_16(uint8_t& dest_hi, uint8_t& dest_lo, uint16_t src);

//------------------- ARITHMETIC -------------------//
Instruction INC_16(uint8_t& hi, uint8_t& lo);
Instruction INC_8(uint8_t& num, FlagRegister& fr);
Instruction INC_8_mem(MMU& mmu, uint16_t addr, FlagRegister& fr);
Instruction ADD_16(uint8_t& num1_hi,uint8_t& num1_lo, uint16_t num2, FlagRegister& fr);
Instruction ADD_16_e8(uint16_t& num1, uint8_t num2, FlagRegister& fr);
Instruction ADD_8_reg(uint8_t& num1, uint8_t& num2, FlagRegister& fr);
Instruction ADD_8_mem(uint8_t& num1, MMU& mmu, uint16_t addr, FlagRegister& fr);
Instruction ADD_8_imm(uint8_t& num1, uint8_t num2, bool carry, FlagRegister& fr);
Instruction ADC_8_reg(uint8_t& num1, uint8_t& num2, FlagRegister& fr);
Instruction ADC_8_mem(uint8_t& num1, MMU& mmu, uint16_t addr, FlagRegister& fr);
Instruction ADC_8_imm(uint8_t& num1, uint8_t num2, FlagRegister& fr);
Instruction SUB_8_reg(uint8_t& num1, uint8_t& num2, FlagRegister& fr);
Instruction SUB_8_mem(uint8_t& num1, MMU& mmu, uint16_t addr, FlagRegister& fr);
Instruction SUB_8_imm(uint8_t& num1, uint8_t num2, FlagRegister& fr);
Instruction SBB_8_reg(uint8_t& num1, uint8_t& num2, FlagRegister& fr);
Instruction SBB_8_mem(uint8_t& num1, MMU& mmu, uint16_t addr, FlagRegister& fr);
Instruction SBB_8_imm(uint8_t& num1, uint8_t num2, FlagRegister& fr);
Instruction DEC_8(uint8_t& num, FlagRegister& fr);
Instruction DEC_8_mem(MMU& mmu, uint16_t addr, FlagRegister& fr);
Instruction DEC_16(uint8_t& hi, uint8_t& lo);  

//------------------LOGICAL------------------//
Instruction AND_8_reg(uint8_t& num1, uint8_t& num2, FlagRegister& fr);
Instruction AND_8_mem(uint8_t& num1, MMU& mmu, uint16_t addr, FlagRegister& fr);
Instruction AND_8_imm(uint8_t& num1, uint8_t num2, FlagRegister& fr);

Instruction OR_8_reg(uint8_t& num1, uint8_t& num2, FlagRegister& fr);
Instruction OR_8_mem(uint8_t& num1, MMU& mmu, uint16_t addr, FlagRegister& fr);
Instruction OR_8_imm(uint8_t& num1, uint8_t num2, FlagRegister& fr);
Instruction XOR_8_reg(uint8_t& num1, uint8_t& num2, FlagRegister& fr);
Instruction XOR_8_mem(uint8_t& num1, MMU& mmu, uint16_t addr, FlagRegister& fr);
Instruction XOR_8_imm(uint8_t& num1, uint8_t num2, FlagRegister& fr);
Instruction CP_8_reg(uint8_t& num1, uint8_t& num2, FlagRegister& fr);
Instruction CP_8_mem(uint8_t& num1, MMU& mmu, uint16_t addr, FlagRegister& fr);
Instruction CP_8_imm(uint8_t& num1, uint8_t num2, FlagRegister& fr);
}   //Operation

#endif