#ifndef INSTRUCTION_H
#define INSTRUCTION

#include <string>
#include <iostream>
#include <functional>
#include <stdint.h>
#include "Register.h"
#include "Arithmetic.h"

namespace SM83 {

class CPU;

class Instruction {
public:
    typedef std::function<void()> Opfn;

    Instruction()
        :op{[](){}}, len{1}, cycles{4}, opname{"NOP"} {}
    Instruction(Opfn operation, int length, int cycles, std::string opname = "")
        :op{std::move(operation)}, len{length}, cycles{0}, opname{opname} {}
    
    void print() {std::cout << opname << '\n';}
    void execute() { op(); }
    int length() {return len;}

private:
    Opfn op;
    int len;
    int cycles;
    std::string opname;
};

namespace Operation {
//basic operations
inline Instruction::Opfn load8(uint8_t& dest, uint8_t src) {
    return [&dest, src]() {dest = src;};
}
inline Instruction::Opfn load16(uint8_t& dest_hi, uint8_t& dest_lo, uint8_t src_hi, uint8_t src_lo) {
    return [&dest_hi, &dest_lo, src_hi, src_lo]() {
        dest_hi = src_hi;
        dest_lo = src_lo;
    };
}
inline Instruction::Opfn inc16(uint8_t& hi, uint8_t& lo) {
    return [&hi, &lo](){
        uint16_t pair = Arithmetic::pair(hi,lo);
        pair++;
        hi = pair >> 8;
        lo = pair & 0xff;
    };
}
inline Instruction::Opfn inc8(uint8_t& num, FlagRegister& fr) {
    return [&num, &fr]() {
        uint16_t result = num+1;
        fr.set_flag(Flag::ZERO, (result&0xff) == 0);
        fr.set_flag(Flag::NEGATIVE, 0);
        fr.set_flag(Flag::HALF_CARRY, Arithmetic::half_carry_add(num, 1));
        num = result&0xff;
    };
}
inline Instruction::Opfn add16(uint8_t& num1_hi,uint8_t& num1_lo, uint8_t num2_hi,uint8_t num2_lo, FlagRegister& fr) {
    return [&num1_hi, &num1_lo, num2_hi, num2_lo, &fr]() {
        uint16_t num1 = Arithmetic::pair(num1_hi, num1_lo);
        uint16_t num2 = Arithmetic::pair(num2_hi, num2_lo);
        int result = num1 + num2;
        fr.set_flag(Flag::NEGATIVE, 0);
        fr.set_flag(Flag::HALF_CARRY, Arithmetic::half_carry_add(num1, num2));
        fr.set_flag(Flag::CARRY, result > 0xffff);
        num1_hi = (result & 0xffff) >> 8;
        num1_lo = result & 0xff;
    };
} 
inline Instruction::Opfn add8(uint8_t& num1, uint8_t num2, bool carry, FlagRegister& fr) {
    return [&num1, num2, carry, &fr]() {
        uint16_t result = num1 + num2 + (uint8_t)carry;
        fr.set_flag(Flag::ZERO, (result&0xff) == 0);
        fr.set_flag(Flag::NEGATIVE, 0);
        fr.set_flag(Flag::HALF_CARRY, Arithmetic::half_carry_add(num1, num2+carry));
        fr.set_flag(Flag::CARRY, result > 0xff);
        num1 = result & 0xff;
    };
}
inline Instruction::Opfn sub8(uint8_t& num1, uint8_t num2, bool carry, FlagRegister& fr) {
    return [&num1, num2, carry, &fr]() {
        uint16_t result = num1 - num2 - (uint8_t)carry;
        fr.set_flag(Flag::ZERO, (result&0xff) == 0);
        fr.set_flag(Flag::NEGATIVE, 1);
        fr.set_flag(Flag::HALF_CARRY, Arithmetic::half_carry_sub(num1, num2+carry));
        fr.set_flag(Flag::CARRY, num2+carry > num1);
        num1 = result & 0xff;
    };
}
inline Instruction::Opfn sub16(uint8_t& num1_hi,uint8_t& num1_lo, uint8_t num2_hi,uint8_t num2_lo, FlagRegister& fr) {
    return [&num1_hi, &num1_lo, num2_hi, num2_lo, &fr]() {
        uint16_t num1 = Arithmetic::pair(num1_hi, num1_lo);
        uint16_t num2 = Arithmetic::pair(num2_hi, num2_lo);
        int result = num1 - num2;
        fr.set_flag(Flag::NEGATIVE, 1);
        fr.set_flag(Flag::HALF_CARRY, Arithmetic::half_carry_sub(num1, num2));
        fr.set_flag(Flag::CARRY, num2 > num1);
        num1_hi = (result & 0xffff) >> 8;
        num1_lo = result & 0xff;
    };
} 
inline Instruction::Opfn dec8(uint8_t& num, FlagRegister& fr){
    return [&num, &fr]() {
        uint16_t result = num-1;
        fr.set_flag(Flag::ZERO, (result&0xff) == 0);
        fr.set_flag(Flag::NEGATIVE, 1);
        fr.set_flag(Flag::HALF_CARRY, Arithmetic::half_carry_sub(num, 1));
        num = result&0xff;
    };
}
inline Instruction::Opfn dec16(uint8_t& hi, uint8_t& lo) {
    return [&hi, &lo](){
        uint16_t pair = Arithmetic::pair(hi, lo);
        pair--;
        hi = pair >> 8;
        lo = pair & 0xff;
    };
}
}   //Operation

}   //SM83

#endif