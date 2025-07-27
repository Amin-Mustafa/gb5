#include "../include/Arithmetic.h"
#include "../include/Register.h"
#include "../include/Instruction.h"
#include "../include/MMU.h"

namespace Operation {

Instruction NOP() {
    return Instruction{};
}

Instruction LD_8(Register8& dest, const Register8& src) {
    return Instruction { 
        [&](){dest.set( src.get() );}
    };
}
Instruction LD_16(Register16& dest, const Register16& src) {
    return Instruction {
        [&](){dest.set( src.get() );}
    };
}

//------------------- ARITHMETIC -------------------//
Instruction INC_16(Register16& rp) {
    return Instruction {
        [&](){ rp.set(rp.get() + 1); }
    };
}
Instruction INC_8(Register8& num, FlagRegister& fr) {
    return Instruction{ 
        [&](){
            uint16_t result = num.get() + 1;
            fr.set_flag(Flag::ZERO, (result&0xff) == 0);
            fr.set_flag(Flag::NEGATIVE, 0);
            fr.set_flag(Flag::HALF_CARRY, Arithmetic::half_carry_add(num.get(), 1));
            num.set(result&0xff);
        }
    };
}
Instruction ADD_16(Register16& num1, const Register16& num2, FlagRegister& fr) {
    return Instruction{
        [&]() {
            uint16_t a = num1.get();
            uint16_t b = num2.get();
            int result = a + b;
            fr.set_flag(Flag::NEGATIVE, 0);
            fr.set_flag(Flag::HALF_CARRY, Arithmetic::half_carry_add(a, b));
            fr.set_flag(Flag::CARRY, result > 0xffff);
            num1.set(result);
        },
        4   
    };
} 
Instruction ADD_SP_e8(StackPointer& num1, const Immediate8& num2, FlagRegister& fr){
    return Instruction{
        [&](){
            uint8_t a = num1.get();
            int8_t b = static_cast<int8_t>(num2.get());
            uint16_t result = a + b;
            bool hcarry = b >= 0 ? Arithmetic::half_carry_add(a, b) : 
                                    Arithmetic::half_carry_sub(a, b);
            bool carry = b >= 0 ? result > 0xffff : b > a;
            fr.set_flag(Flag::ZERO, 0);
            fr.set_flag(Flag::NEGATIVE, 0);
            fr.set_flag(Flag::HALF_CARRY, hcarry );
            fr.set_flag(Flag::CARRY, carry);
            num1.set(result);
        },
        8  
    };
    //initial fetch (4) + operand fetch (4) + 16-bit add (4) + stack modification (4)
    //Total cycles: 16
}
Instruction ADD_8(Register8& num1, const Register8& num2, FlagRegister& fr) {
    return Instruction { 
        [&](){ 
            uint8_t a = num1.get();
            uint8_t b = num2.get();
            uint16_t result = a + b;
            fr.set_flag(Flag::ZERO, (result&0xff) == 0);
            fr.set_flag(Flag::NEGATIVE, 0);
            fr.set_flag(Flag::HALF_CARRY, Arithmetic::half_carry_add(a, b));
            fr.set_flag(Flag::CARRY, result > 0xff);
            num1.set(result & 0xff);
        }
    };
}
Instruction ADC_8(Register8& num1, const Register8& num2, FlagRegister& fr) {
   return Instruction { 
        [&](){ 
            bool carry = fr.get_flag(Flag::CARRY);
            uint8_t a = num1.get();
            uint8_t b = num2.get();
            uint16_t result = a + b + carry;
            fr.set_flag(Flag::ZERO, (result&0xff) == 0);
            fr.set_flag(Flag::NEGATIVE, 0);
            fr.set_flag(Flag::HALF_CARRY, Arithmetic::half_carry_add(a, b + carry));
            fr.set_flag(Flag::CARRY, result > 0xff);
            num1.set(result & 0xff);
        }
    };
}

Instruction SUB_8(Register8& num1, const Register8& num2, FlagRegister& fr) {
    return Instruction { 
        [&](){
            uint8_t a = num1.get();
            uint8_t b = num2.get();
            uint16_t result = a - b;
            fr.set_flag(Flag::ZERO, (result&0xff) == 0);
            fr.set_flag(Flag::NEGATIVE, 1);
            fr.set_flag(Flag::HALF_CARRY, Arithmetic::half_carry_sub(a, b));
            fr.set_flag(Flag::CARRY, b > a);
            num1.set(result & 0xff);
        }
    };
}
Instruction SBC_8(Register8& num1, const Register8& num2, FlagRegister& fr) {
    return Instruction { 
        [&](){
            uint8_t a = num1.get();
            uint8_t b = num2.get();
            bool carry = fr.get_flag(Flag::CARRY);
            uint16_t result = a - b - carry;
            fr.set_flag(Flag::ZERO, (result&0xff) == 0);
            fr.set_flag(Flag::NEGATIVE, 1);
            fr.set_flag(Flag::HALF_CARRY, Arithmetic::half_carry_sub(a, b+carry));
            fr.set_flag(Flag::CARRY, b + carry > a);
            num1.set(result & 0xff);
        }
    };
}
Instruction DEC_8(Register8& num, FlagRegister& fr){
    return Instruction{ 
        [&](){
            uint16_t result = num.get() - 1;
            fr.set_flag(Flag::ZERO, (result&0xff) == 0);
            fr.set_flag(Flag::NEGATIVE, 1);
            fr.set_flag(Flag::HALF_CARRY, Arithmetic::half_carry_sub(num.get(), 1));
            num.set(result&0xff);
        }
    };
}
Instruction DEC_16(Register16& rp) {
    return Instruction{
        [&](){
            rp.set(rp.get() - 1);
        }
    };
}     
Instruction RLC_8(Register8& num, FlagRegister& fr){
    return Instruction{
        [&]() {
            bool carry = fr.get_flag(Flag::CARRY);
            uint8_t result = Arithmetic::rot_left_circ(num.get(), carry);
            fr.set(flag_state(!result, 0, 0, carry));
            num.set(result);
        }
    };
}
Instruction RRC_8(Register8& num, FlagRegister& fr){ 
    return Instruction{
        [&]() {
            bool carry = fr.get_flag(Flag::CARRY);
            uint8_t result = Arithmetic::rot_right_circ(num.get(), carry);
            fr.set(flag_state(!result, 0, 0, carry));
            num.set(result);
        }
    };
}
Instruction RL_8(Register8& num, FlagRegister& fr) {
    return Instruction{
        [&]() {
            bool carry = fr.get_flag(Flag::CARRY);
            uint8_t result = Arithmetic::rot_left(num.get(), carry);
            fr.set(flag_state(!result, 0, 0, carry));
            num.set(result);
        }
    };
}
Instruction RR_8(Register8& num, FlagRegister& fr) {
    return Instruction{
        [&]() {
            bool carry = fr.get_flag(Flag::CARRY);
            uint8_t result = Arithmetic::rot_right(num.get(), carry);
            fr.set(flag_state(!result, 0, 0, carry));
            num.set(result);
        }
    };
}
Instruction SLA_8(Register8& num, FlagRegister& fr) {
    return Instruction{
        [&]() {
            bool carry = fr.get_flag(Flag::CARRY);
            uint8_t result = Arithmetic::shift_left_arithmetic(num.get(), carry);
            fr.set(flag_state(!result, 0, 0, carry));
            num.set(result);
        }
    };
}
Instruction SRA_8(Register8& num, FlagRegister& fr){ 
    return Instruction{
        [&]() {
            bool carry = fr.get_flag(Flag::CARRY);
            uint8_t result = Arithmetic::shift_right_arithmetic(num.get(), carry);
            fr.set(flag_state(!result, 0, 0, carry));
            num.set(result);
        }
    };
}
Instruction SRL_8(Register8& num, FlagRegister& fr) {
    return Instruction{
        [&]() {
            bool carry = fr.get_flag(Flag::CARRY);
            uint8_t result = Arithmetic::shift_right_logical(num.get(), carry);
            fr.set(flag_state(!result, 0, 0, carry));
            num.set(result);
        }
    };
}


//------------------LOGICAL------------------//
Instruction AND_8(Register8& num1, const Register8& num2, FlagRegister& fr) {
    return Instruction{
        [&](){
            num1.set(num1.get() & num2.get());
            fr = flag_state(num1.get() == 0, 0, 1, 0);
        }
    };
}
Instruction OR_8(Register8& num1, const Register8& num2, FlagRegister& fr) {
    return Instruction{
        [&](){
            num1.set(num1.get() | num2.get());
            fr = flag_state(num1.get() == 0, 0, 0, 0);
        }
    };
}

Instruction XOR_8(Register8& num1, const Register8& num2, FlagRegister& fr) {
    return Instruction{
        [&](){
            num1.set(num1.get() ^ num2.get());
            fr = flag_state(num1.get() == 0, 0, 0, 0);
        }
    };
}
Instruction CP_8(Register8& num1, const Register8& num2, FlagRegister& fr) {
    return Instruction { 
        [&](){
            uint8_t a = num1.get();
            uint8_t b = num2.get();
            uint16_t result = a - b;
            fr.set_flag(Flag::ZERO, (result&0xff) == 0);
            fr.set_flag(Flag::NEGATIVE, 1);
            fr.set_flag(Flag::HALF_CARRY, Arithmetic::half_carry_sub(a, b));
            fr.set_flag(Flag::CARRY, b > a);
            //no set
        } 
    };
}
}   //Operation