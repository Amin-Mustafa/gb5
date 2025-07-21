#include "../include/Arithmetic.h"
#include "../include/Register.h"
#include "../include/Instruction.h"
#include "../include/MMU.h"

namespace Operation {

Instruction LD_8(Register& dest, Register& src) {
    return Instruction { 
        [&](){dest.set( src.get() );},
        1, 4
    };
}
Instruction LD_16(RegisterPair& dest, uint16_t src) {
    return Instruction {
        [&]() {dest.set(src);}, 
        3,12
    };
}
Instruction LD_16(uint16_t& dest, uint16_t src) {
    return Instruction {
        [&]() {dest = src;}, 
        3,12
    };
}


//------------------- ARITHMETIC -------------------//
Instruction INC_16(RegisterPair& rp) {
    return Instruction {
        [&](){ rp.set(rp.get() + 1); }, 
        1,8
    };
}
Instruction INC_8(Register& num, FlagRegister& fr) {
    return Instruction{ 
        [&](){
            uint16_t result = num.get() + 1;
            fr.set_flag(Flag::ZERO, (result&0xff) == 0);
            fr.set_flag(Flag::NEGATIVE, 0);
            fr.set_flag(Flag::HALF_CARRY, Arithmetic::half_carry_add(num.get(), 1));
            num.set(result&0xff);
        },
        1, 4
    };
}
Instruction ADD_16(RegisterPair& num1, uint16_t num2, FlagRegister& fr) {
    return Instruction{
        [&]() {
            int result = num1.get() + num2;
            fr.set_flag(Flag::NEGATIVE, 0);
            fr.set_flag(Flag::HALF_CARRY, Arithmetic::half_carry_add(num1.get(), num2));
            fr.set_flag(Flag::CARRY, result > 0xffff);
            num1.set(result);
        }, 
        1,8
    };
} 
Instruction ADD_16_e8(RegisterPair& num1, uint8_t num2, FlagRegister& fr){
    return Instruction{
        [&](){
            int8_t addend = static_cast<int8_t>(num2);
            uint16_t result = num1.get() + addend;
            bool hcarry = addend >= 0 ? Arithmetic::half_carry_add(num1.get(), addend) : 
                                    Arithmetic::half_carry_sub(num1.get(), addend);
            bool carry = addend >= 0 ? result > 0xffff : addend > num1.get();
            fr.set_flag(Flag::ZERO, 0);
            fr.set_flag(Flag::NEGATIVE, 0);
            fr.set_flag(Flag::HALF_CARRY, hcarry );
            fr.set_flag(Flag::CARRY, carry);
            num1.set(result);
        },
        2,16
    };
}
Instruction ADD_8(Register& num1, uint8_t num2, FlagRegister& fr) {
    return Instruction { 
        [&](){ 
            uint16_t result = num1.get() + num2;
            fr.set_flag(Flag::ZERO, (result&0xff) == 0);
            fr.set_flag(Flag::NEGATIVE, 0);
            fr.set_flag(Flag::HALF_CARRY, Arithmetic::half_carry_add(num1.get(), num2));
            fr.set_flag(Flag::CARRY, result > 0xff);
            num1.set(result & 0xff);
        },
        1,4
    };
}
Instruction ADC_8(Register& num1, uint8_t& num2, FlagRegister& fr) {
   return Instruction { 
        [&](){ 
            bool carry = fr.get_flag(Flag::CARRY);
            uint16_t result = num1.get() + num2 + carry;
            fr.set_flag(Flag::ZERO, (result&0xff) == 0);
            fr.set_flag(Flag::NEGATIVE, 0);
            fr.set_flag(Flag::HALF_CARRY, Arithmetic::half_carry_add(num1.get(), num2 + carry));
            fr.set_flag(Flag::CARRY, result > 0xff);
            num1.set(result & 0xff);
        },
        1,4
    };
}

Instruction SUB_8(Register& num1, uint8_t& num2, FlagRegister& fr) {
    return Instruction { 
        [&](){
            uint16_t result = num1.get() - num2;
            fr.set_flag(Flag::ZERO, (result&0xff) == 0);
            fr.set_flag(Flag::NEGATIVE, 1);
            fr.set_flag(Flag::HALF_CARRY, Arithmetic::half_carry_sub(num1.get(), num2));
            fr.set_flag(Flag::CARRY, num2 > num1.get());
            num1.set(result & 0xff);
        },
        1,4 
    };
}
Instruction SBB_8(Register& num1, uint8_t& num2, FlagRegister& fr) {
    return Instruction { 
        [&](){
            bool carry = fr.get_flag(Flag::CARRY);
            uint16_t result = num1.get() - num2 - carry;
            fr.set_flag(Flag::ZERO, (result&0xff) == 0);
            fr.set_flag(Flag::NEGATIVE, 1);
            fr.set_flag(Flag::HALF_CARRY, Arithmetic::half_carry_sub(num1.get(), num2+carry));
            fr.set_flag(Flag::CARRY, num2 + carry > num1.get());
            num1.set(result & 0xff);
        },
        1,4 
    };
}
Instruction DEC_8(Register& num, FlagRegister& fr){
    return Instruction{ 
        [&](){
            uint16_t result = num.get() - 1;
            fr.set_flag(Flag::ZERO, (result&0xff) == 0);
            fr.set_flag(Flag::NEGATIVE, 1);
            fr.set_flag(Flag::HALF_CARRY, Arithmetic::half_carry_sub(num.get(), 1));
            num.set(result&0xff);
        },
        1,4 
    };
}
Instruction DEC_16(RegisterPair& rp) {
    return Instruction{
        [&](){
            rp.set(rp.get() - 1);
        }, 
        1, 8
    };
}      

//------------------LOGICAL------------------//
Instruction AND_8(Register& num1, uint8_t num2, FlagRegister& fr) {
    return Instruction{
        [&](){
            num1.set(num1.get() & num2);
            fr = flag_state(num1.get() == 0, 0, 1, 0);
        },
        1,4
    };
}
Instruction OR_8(Register& num1, uint8_t num2, FlagRegister& fr) {
    return Instruction{
        [&](){
            num1.set(num1.get() | num2);
            fr = flag_state(num1.get() == 0, 0, 0, 0);
        },
        1,4
    };
}

Instruction XOR_8(Register& num1, uint8_t num2, FlagRegister& fr) {
    return Instruction{
        [&](){
            num1.set(num1.get() ^ num2);
            fr = flag_state(num1.get() == 0, 0, 0, 0);
        },
        1,4
    };
}
Instruction CP_8(Register& num1, uint8_t num2, FlagRegister& fr) {
    return Instruction { 
        [&](){
            uint16_t result = num1.get() - num2;
            fr.set_flag(Flag::ZERO, (result&0xff) == 0);
            fr.set_flag(Flag::NEGATIVE, 1);
            fr.set_flag(Flag::HALF_CARRY, Arithmetic::half_carry_sub(num1.get(), num2));
            fr.set_flag(Flag::CARRY, num2 > num1.get());
            //leave num1 unchanged
        },
        1,4 
    };
}
}   //Operation