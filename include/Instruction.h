#ifndef INSTRUCTION_H
#define INSTRUCTION_H

#include <string>
#include <iostream>
#include <functional>
#include <cstdint>

class MMU;
class Register8;
class Register16;
class FlagRegister;
class StackPointer;
class Immediate8;

class Instruction {
public:
    typedef std::function<void()> Opfn;

    Instruction()
        :op{[](){}} {}  //NOP
    Instruction(Opfn operation, int duration = 0)
        :op{std::move(operation)}, cycles{duration} {}
    
    void execute() { op(); }
private:
    Opfn op;
    int cycles;
};

namespace Operation {
Instruction NOP();
//------------------------ LOADS ------------------------//
Instruction LD_8(Register8& dest, const Register8& src);
Instruction LD_16(Register16& dest, const Register16& src);

//------------------- ARITHMETIC -------------------//
//8-bit
Instruction INC_8(Register8& num, FlagRegister& fr);
Instruction ADD_8(Register8& num1, const Register8& num2, FlagRegister& fr);
Instruction ADC_8(Register8& num1, const Register8& num2, FlagRegister& fr);
Instruction SUB_8(Register8& num1, const Register8& num2, FlagRegister& fr);
Instruction SBC_8(Register8& num1, const Register8& num2, FlagRegister& fr);
Instruction DEC_8(Register8& num, FlagRegister& fr);
Instruction RLC_8(Register8& num, FlagRegister& fr);
Instruction RRC_8(Register8& num, FlagRegister& fr);
Instruction RL_8(Register8& num, FlagRegister& fr);
Instruction RR_8(Register8& num, FlagRegister& fr);
Instruction SLA_8(Register8& num, FlagRegister& fr);
Instruction SRA_8(Register8& num, FlagRegister& fr);
Instruction SRL_8(Register8& num, FlagRegister& fr);
//16-bit
Instruction INC_16(Register16& num);
Instruction ADD_16(Register16& num, const Register16& num2, FlagRegister& fr);
Instruction ADD_SP_e8(StackPointer& num1, const Immediate8& num2, FlagRegister& fr);
Instruction DEC_16(Register16& num);  

//------------------LOGICAL------------------//
Instruction AND_8(Register8& num1, const Register8& num2, FlagRegister& fr);
Instruction OR_8 (Register8& num1, const Register8& num2, FlagRegister& fr);
Instruction XOR_8(Register8& num1, const Register8& num2, FlagRegister& fr);
Instruction CP_8 (Register8& num1, const Register8& num2, FlagRegister& fr);
}   //Operation

#endif