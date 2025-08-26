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

class CPU;

class Instruction {
public:
    typedef std::function<void(CPU&)> Opfn;

    Instruction(std::initializer_list<Opfn> sub_ops)
        :ops{sub_ops} {}

    int length() const {return ops.size();}
    void execute_subop(CPU& cpu, int index) { ops[index](cpu); }
private:
    //each instruction is divided into atomic sub-operations
    //that each take 1 cpu m-cycle
    std::vector<Opfn> ops; 
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

//-----------------PREFIX OPS-----------------//
Instruction RLC(Register8& num, FlagRegister& fr);
Instruction RRC(Register8& num, FlagRegister& fr);
Instruction RL(Register8& num, FlagRegister& fr);
Instruction RR(Register8& num, FlagRegister& fr);
Instruction SLA(Register8& num, FlagRegister& fr);
Instruction SRA(Register8& num, FlagRegister& fr);
Instruction SRL(Register8& num, FlagRegister& fr);
Instruction SWAP(Register8& num, FlagRegister& fr);
Instruction BIT(Register8& num, uint8_t bit, FlagRegister& fr);
Instruction RES(Register8& num, uint8_t bit);
Instruction SET(Register8& num, uint8_t bit);
}   //Operation

#endif