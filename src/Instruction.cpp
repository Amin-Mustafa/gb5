#include "../include/Arithmetic.h"
#include "../include/Register.h"
#include "../include/Instruction.h"
#include "../include/MMU.h"

namespace Operation {

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
        }
    };
} 
Instruction ADD_16_e8(Register16& num1, uint8_t num2, FlagRegister& fr){
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
        }
    };
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