#ifndef INSTRUCTION_H
#define INSTRUCTION_H

#include <string>
#include <iostream>
#include <functional>
#include <cstdint>

class MMU;
class Register;

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
Instruction LD_8(Register& dest, uint8_t src);
Instruction LD_16(RegisterPair& dest, uint16_t src);
Instruction LD_16(uint16_t& dest, uint16_t src);

//------------------- ARITHMETIC -------------------//
//8-bit
Instruction INC_8(Register& num, FlagRegister& fr);
Instruction ADD_8(Register& num1, const Register& num2, FlagRegister& fr);
Instruction ADC_8(Register& num1, const Register& num2, FlagRegister& fr);
Instruction SUB_8(Register& num1, const Register& num2, FlagRegister& fr);
Instruction SBB_8(Register& num1, const Register& num2, FlagRegister& fr);
Instruction DEC_8(Register& num, FlagRegister& fr);
//16-bit
Instruction INC_16(RegisterPair& num);
Instruction ADD_16(RegisterPair& num, uint16_t num2, FlagRegister& fr);
Instruction ADD_16(uint16_t& num, uint16_t num2, FlagRegister& fr);
Instruction ADD_16_e8(uint16_t& num1, uint8_t num2, FlagRegister& fr);
Instruction DEC_16(RegisterPair& hi, uint8_t& lo);  
Instruction DEC_16(uint16_t& hi, uint8_t& lo);  

//------------------LOGICAL------------------//
Instruction AND_8(Register& num1, const Register& num2, FlagRegister& fr);
Instruction OR_8(Register& num1,  const Register& num2, FlagRegister& fr);
Instruction XOR_8(Register& num1, const Register& num2, FlagRegister& fr);
Instruction CP_8(Register& num1,  const Register& num2, FlagRegister& fr);
}   //Operation

#endif