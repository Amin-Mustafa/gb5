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
        :op{[](){}}, len{1}, cycles{4} {}
    Instruction(Opfn operation, int length, int cycles)
        :op{std::move(operation)}, len{length}, cycles{0} {}
    
    void execute() { op(); }
    int length() {return len;}

private:
    Opfn op;
    int len;
    int cycles;
};

namespace Operation {
//Basic Operations
//------------------------ LOADS ------------------------//
inline Instruction LD_8_reg(uint8_t& dest, uint8_t& src) {
    return Instruction {
        [&dest, src]() {dest = src;}, 1, 4
    };
}
inline Instruction LD_8_mem(uint8_t& dest, uint8_t& src) {
    return Instruction {
        [&dest, src]() {dest = src;}, 1, 8
    };
}
inline Instruction LD_8_imm(uint8_t& dest, uint8_t src) {
    return Instruction {
        [&dest, src]() {dest = src;}, 2, 8
    };
}
inline Instruction LD_16(uint8_t& dest_hi, uint8_t& dest_lo, uint8_t src_hi, uint8_t src_lo) {
    return Instruction {
        [&dest_hi, &dest_lo, src_hi, src_lo]() {
            dest_hi = src_hi;
            dest_lo = src_lo;
        }, 
        3,12
    };
}

//------------------- ARITHMETIC -------------------//
inline Instruction INC_16(uint8_t& hi, uint8_t& lo) {
    return Instruction {
        [&hi, &lo](){
            uint16_t pair = Arithmetic::pair(hi,lo);
            pair++;
            hi = pair >> 8;
            lo = pair & 0xff;
        }, 
        1,8
    };
}
inline Instruction INC_8(uint8_t& num, FlagRegister& fr) {
    return Instruction{
        [&num, &fr]() {
            uint16_t result = num+1;
            fr.set_flag(Flag::ZERO, (result&0xff) == 0);
            fr.set_flag(Flag::NEGATIVE, 0);
            fr.set_flag(Flag::HALF_CARRY, Arithmetic::half_carry_add(num, 1));
            num = result&0xff;
        }, 
        1, 4
    };
}
inline Instruction ADD_16(uint8_t& num1_hi,uint8_t& num1_lo, uint8_t num2_hi,uint8_t num2_lo, FlagRegister& fr) {
    return Instruction{
        [&num1_hi, &num1_lo, num2_hi, num2_lo, &fr]() {
            uint16_t num1 = Arithmetic::pair(num1_hi, num1_lo);
            uint16_t num2 = Arithmetic::pair(num2_hi, num2_lo);
            int result = num1 + num2;
            fr.set_flag(Flag::NEGATIVE, 0);
            fr.set_flag(Flag::HALF_CARRY, Arithmetic::half_carry_add(num1, num2));
            fr.set_flag(Flag::CARRY, result > 0xffff);
            num1_hi = (result & 0xffff) >> 8;
            num1_lo = result & 0xff;
        }, 
        1,8
    };
} 
inline Instruction ADD_8_reg(uint8_t& num1, uint8_t& num2, bool carry, FlagRegister& fr) {
    return Instruction {
        [&num1, num2, carry, &fr]() {
            uint16_t result = num1 + num2 + (uint8_t)carry;
            fr.set_flag(Flag::ZERO, (result&0xff) == 0);
            fr.set_flag(Flag::NEGATIVE, 0);
            fr.set_flag(Flag::HALF_CARRY, Arithmetic::half_carry_add(num1, num2+carry));
            fr.set_flag(Flag::CARRY, result > 0xff);
            num1 = result & 0xff;
        }, 
        1,4
    };
}
inline Instruction ADD_8_mem(uint8_t& num1, uint8_t& num2, bool carry, FlagRegister& fr) {
    return Instruction {
        [&num1, num2, carry, &fr]() {
            uint16_t result = num1 + num2 + (uint8_t)carry;
            fr.set_flag(Flag::ZERO, (result&0xff) == 0);
            fr.set_flag(Flag::NEGATIVE, 0);
            fr.set_flag(Flag::HALF_CARRY, Arithmetic::half_carry_add(num1, num2+carry));
            fr.set_flag(Flag::CARRY, result > 0xff);
            num1 = result & 0xff;
        }, 
        1,8
    };
}
inline Instruction ADD_8_imm(uint8_t& num1, uint8_t num2, bool carry, FlagRegister& fr) {
    return Instruction {
        [&num1, num2, carry, &fr]() {
            uint16_t result = num1 + num2 + (uint8_t)carry;
            fr.set_flag(Flag::ZERO, (result&0xff) == 0);
            fr.set_flag(Flag::NEGATIVE, 0);
            fr.set_flag(Flag::HALF_CARRY, Arithmetic::half_carry_add(num1, num2+carry));
            fr.set_flag(Flag::CARRY, result > 0xff);
            num1 = result & 0xff;
        }, 
        2,8
    };
}

inline Instruction SUB_8_reg(uint8_t& num1, uint8_t& num2, bool carry, FlagRegister& fr) {
    return Instruction{
        [&num1, num2, carry, &fr]() {
            uint16_t result = num1 - num2 - (uint8_t)carry;
            fr.set_flag(Flag::ZERO, (result&0xff) == 0);
            fr.set_flag(Flag::NEGATIVE, 1);
            fr.set_flag(Flag::HALF_CARRY, Arithmetic::half_carry_sub(num1, num2+carry));
            fr.set_flag(Flag::CARRY, num2+carry > num1);
            num1 = result & 0xff;
        }, 
        1, 4
    };
}
inline Instruction SUB_8_mem(uint8_t& num1, uint8_t& num2, bool carry, FlagRegister& fr) {
    return Instruction{
        [&num1, num2, carry, &fr]() {
            uint16_t result = num1 - num2 - (uint8_t)carry;
            fr.set_flag(Flag::ZERO, (result&0xff) == 0);
            fr.set_flag(Flag::NEGATIVE, 1);
            fr.set_flag(Flag::HALF_CARRY, Arithmetic::half_carry_sub(num1, num2+carry));
            fr.set_flag(Flag::CARRY, num2+carry > num1);
            num1 = result & 0xff;
        },
        1, 8
    };
}
inline Instruction SUB_8_imm(uint8_t& num1, uint8_t num2, bool carry, FlagRegister& fr) {
    return Instruction{
        [&num1, num2, carry, &fr]() {
            uint16_t result = num1 - num2 - (uint8_t)carry;
            fr.set_flag(Flag::ZERO, (result&0xff) == 0);
            fr.set_flag(Flag::NEGATIVE, 1);
            fr.set_flag(Flag::HALF_CARRY, Arithmetic::half_carry_sub(num1, num2+carry));
            fr.set_flag(Flag::CARRY, num2+carry > num1);
            num1 = result & 0xff;
        },
        2, 8
    };
}
inline Instruction DEC_8(uint8_t& num, FlagRegister& fr){
    return Instruction{
        [&num, &fr]() {
            uint16_t result = num-1;
            fr.set_flag(Flag::ZERO, (result&0xff) == 0);
            fr.set_flag(Flag::NEGATIVE, 1);
            fr.set_flag(Flag::HALF_CARRY, Arithmetic::half_carry_sub(num, 1));
            num = result&0xff;
        },
        1,4
    };
}
inline Instruction DEC_16(uint8_t& hi, uint8_t& lo) {
    return Instruction{
        [&hi, &lo](){
            uint16_t pair = Arithmetic::pair(hi, lo);
            pair--;
            hi = pair >> 8;
            lo = pair & 0xff;
        }, 
        1, 8
    };
}      

//------------------LOGICAL------------------//
inline Instruction AND_8_reg(uint8_t& num1, uint8_t& num2, FlagRegister& fr) {
    return Instruction{
        [&num1, num2, &fr]() {
            num1 &= num2;
            fr = flag_state(num1 == 0, 0, 1, 0);
        }, 
        1,4
    };
}
inline Instruction OR_8_reg(uint8_t& num1, uint8_t& num2, FlagRegister& fr) {
    return Instruction{
        [&num1, num2, &fr]() {
            num1 |= num2;
            fr = flag_state(num1 == 0, 0, 0, 0);
        }, 
        1,4
    };
}
inline Instruction XOR_8_reg(uint8_t& num1, uint8_t& num2, FlagRegister& fr) {
    return Instruction{
        [&num1, num2, &fr]() {
            num1 ^= num2;
            fr = flag_state(num1 == 0, 0, 0, 0);
        }, 
        1,4
    };
}
inline Instruction CP_8_reg(uint8_t& num1, uint8_t& num2, FlagRegister& fr) {
    return Instruction{
        [&num1, num2, &fr]() {
            uint16_t result = num1 - num2;
            fr.set_flag(Flag::ZERO, (result&0xff) == 0);
            fr.set_flag(Flag::NEGATIVE, 1);
            fr.set_flag(Flag::HALF_CARRY, Arithmetic::half_carry_sub(num1, num2));
            fr.set_flag(Flag::CARRY, num2 > num1);
        },
        1, 4
    };
}
inline Instruction CP_8_mem(uint8_t& num1, uint8_t& num2, FlagRegister& fr) {
    return Instruction{
        [&num1, num2, &fr]() {
            uint16_t result = num1 - num2;
            fr.set_flag(Flag::ZERO, (result&0xff) == 0);
            fr.set_flag(Flag::NEGATIVE, 1);
            fr.set_flag(Flag::HALF_CARRY, Arithmetic::half_carry_sub(num1, num2));
            fr.set_flag(Flag::CARRY, num2 > num1);
        },
        1, 8
    };
}
inline Instruction CP_8_imm(uint8_t& num1, uint8_t num2, FlagRegister& fr) {
    return Instruction{
        [&num1, num2, &fr]() {
            uint16_t result = num1 - num2;
            fr.set_flag(Flag::ZERO, (result&0xff) == 0);
            fr.set_flag(Flag::NEGATIVE, 1);
            fr.set_flag(Flag::HALF_CARRY, Arithmetic::half_carry_sub(num1, num2));
            fr.set_flag(Flag::CARRY, num2 > num1);
        },
        2, 8
    };
}
}   //Operation

}   //SM83

#endif