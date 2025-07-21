#include "../include/Arithmetic.h"
#include "../include/Register.h"
#include "../include/Instruction.h"
#include "../include/MMU.h"

namespace Operation {

    namespace Primitive {
    inline void load_8_bit(uint8_t& dest, uint8_t& src){
        dest = src;
    }
    inline void inc_8_bit(uint8_t& num, FlagRegister& fr){
        uint16_t result = num+1;
        fr.set_flag(Flag::ZERO, (result&0xff) == 0);
        fr.set_flag(Flag::NEGATIVE, 0);
        fr.set_flag(Flag::HALF_CARRY, Arithmetic::half_carry_add(num, 1));
        num = result&0xff;
    }
    inline void dec_8_bit(uint8_t& num, FlagRegister& fr) {
        uint16_t result = num-1;
        fr.set_flag(Flag::ZERO, (result&0xff) == 0);
        fr.set_flag(Flag::NEGATIVE, 1);
        fr.set_flag(Flag::HALF_CARRY, Arithmetic::half_carry_sub(num, 1));
        num = result&0xff;
    }
    inline void add_8_bit(uint8_t& num1, uint8_t num2, bool carry, FlagRegister& fr){
        uint16_t result = num1 + num2 + (uint8_t)carry;
        fr.set_flag(Flag::ZERO, (result&0xff) == 0);
        fr.set_flag(Flag::NEGATIVE, 0);
        fr.set_flag(Flag::HALF_CARRY, Arithmetic::half_carry_add(num1, num2+carry));
        fr.set_flag(Flag::CARRY, result > 0xff);
        num1 = result & 0xff;
    }
    inline void sub_8_bit(uint8_t& num1, uint8_t num2, bool carry, FlagRegister& fr){
        uint16_t result = num1 - num2 - (uint8_t)carry;
        fr.set_flag(Flag::ZERO, (result&0xff) == 0);
        fr.set_flag(Flag::NEGATIVE, 1);
        fr.set_flag(Flag::HALF_CARRY, Arithmetic::half_carry_sub(num1, num2+carry));
        fr.set_flag(Flag::CARRY, num2+carry > num1);
        num1 = result & 0xff;
    }
    inline void and_8_bit(uint8_t& num1, uint8_t num2, FlagRegister& fr){
        num1 &= num2;
        fr = flag_state(num1 == 0, 0, 1, 0);
    }
    inline void or_8_bit(uint8_t& num1, uint8_t num2, FlagRegister& fr){
        num1 |= num2;
        fr = flag_state(num1 == 0, 0, 0, 0);
    }
    inline void xor_8_bit(uint8_t& num1, uint8_t num2, FlagRegister& fr){
        num1 ^= num2;
        fr = flag_state(num1 == 0, 0, 0, 0);
    }
    inline void compare_8_bit(uint8_t num1, uint8_t num2, FlagRegister fr){
        sub_8_bit(num1, num2, 0, fr);    //by-value sub; none of the arguments changed
    }
    
    } //Primitive

Instruction LD_8_reg(uint8_t& dest, uint8_t& src) {
    return Instruction { 
        [&](){Primitive::load_8_bit(dest,src);},
        1, 4
    };
}
Instruction LD_8_mem_r(MMU& mmu, uint16_t addr, uint8_t& src) {
    return Instruction {
        [&](){mmu.write(addr, src);},
        1, 8
    };
}
Instruction LD_8_r_mem(uint8_t& dest, MMU& mmu, uint16_t addr) {
    return Instruction {
        [&](){dest = mmu.read(addr);},
        1, 8
    };
}
Instruction LD_8_imm(uint8_t& dest, uint8_t src) {
    return Instruction {
        [&](){ Primitive::load_8_bit(dest, src); },
        2, 8
    };
}
Instruction LD_16(uint8_t& dest_hi, uint8_t& dest_lo, uint16_t src) {
    return Instruction {
        [&]() {
            dest_hi = src >> 8;
            dest_lo = src&0xff;
        }, 
        3,12
    };
}

//------------------- ARITHMETIC -------------------//
Instruction INC_16(uint8_t& hi, uint8_t& lo) {
    return Instruction {
        [&](){
            uint16_t pair = Arithmetic::pair(hi,lo);
            pair++;
            hi = pair >> 8;
            lo = pair & 0xff;
        }, 
        1,8
    };
}
Instruction INC_8(uint8_t& num, FlagRegister& fr) {
    return Instruction{ 
        [&](){ Primitive::inc_8_bit(num, fr); },
        1, 4
    };
}
Instruction INC_8_mem(MMU& mmu, uint16_t addr, FlagRegister& fr) {
    return Instruction{ 
        [&](){
            uint8_t temp = mmu.read(addr);
            Primitive::inc_8_bit(temp, fr);
            mmu.write(addr, temp);
        },
        1, 12
     };
}
Instruction ADD_16(uint8_t& num1_hi,uint8_t& num1_lo, uint16_t num2, FlagRegister& fr) {
    return Instruction{
        [&]() {
            uint16_t num1 = Arithmetic::pair(num1_hi, num1_lo);
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
Instruction ADD_16_e8(uint16_t& num1, uint8_t num2, FlagRegister& fr){
    return Instruction{
        [&](){
            int8_t addend = static_cast<int8_t>(num2);
            uint16_t res = num1 + addend;
            bool hcarry = addend >= 0 ? Arithmetic::half_carry_add(num1, addend) : 
                                    Arithmetic::half_carry_sub(num1, addend);
            bool carry = addend >= 0 ? res > 0xffff : addend > num1;
            fr.set_flag(Flag::ZERO, 0);
            fr.set_flag(Flag::NEGATIVE, 0);
            fr.set_flag(Flag::HALF_CARRY, hcarry );
            fr.set_flag(Flag::CARRY, carry);
        },
        2,16
    };
}
Instruction ADD_8_reg(uint8_t& num1, uint8_t& num2, FlagRegister& fr) {
    return Instruction { 
        [&](){ Primitive::add_8_bit(num1, num2, 0, fr); },
        1,4
    };
}
Instruction ADD_8_mem(uint8_t& num1, MMU& mmu, uint16_t addr, FlagRegister& fr) {
    return Instruction { 
        [&](){
            uint8_t num2 = mmu.read(addr);
            Primitive::add_8_bit(num1, num2, 0, fr);
            mmu.write(addr, num1);
        },
        1,8
    };
}
Instruction ADD_8_imm(uint8_t& num1, uint8_t num2, bool carry, FlagRegister& fr) {
    return Instruction { 
        [&](){Primitive::add_8_bit(num1, num2, 0, fr);}, 
        2,8 
    };
}
Instruction ADC_8_reg(uint8_t& num1, uint8_t& num2, FlagRegister& fr) {
    return Instruction { 
        [&](){ Primitive::add_8_bit(num1, num2, fr.get_flag(Flag::CARRY), fr); },
        1,4
    };
}
Instruction ADC_8_mem(uint8_t& num1, MMU& mmu, uint16_t addr, FlagRegister& fr) {
    return Instruction { 
        [&](){
            uint8_t num2 = mmu.read(addr);
            Primitive::add_8_bit(num1, num2, fr.get_flag(Flag::CARRY), fr);
            mmu.write(addr, num1);
        },
        1,8
    };
}
Instruction ADC_8_imm(uint8_t& num1, uint8_t num2, FlagRegister& fr) {
    return Instruction { 
        [&](){Primitive::add_8_bit(num1, num2, fr.get_flag(Flag::CARRY), fr);}, 
        2,8 
    };
}

Instruction SUB_8_reg(uint8_t& num1, uint8_t& num2, FlagRegister& fr) {
    return Instruction{ 
        [&](){Primitive::sub_8_bit(num1, num2, 0, fr);}, 
        1,4 
    };
}
Instruction SUB_8_mem(uint8_t& num1, MMU& mmu, uint16_t addr, FlagRegister& fr) {
    return Instruction{
        [&](){
            uint8_t num2 = mmu.read(addr);
            Primitive::sub_8_bit(num1, num2, 0, fr);
            mmu.write(addr, num1);
        },
        1,8 
    };
}
Instruction SUB_8_imm(uint8_t& num1, uint8_t num2, FlagRegister& fr) {
    return Instruction{ 
        [&](){Primitive::sub_8_bit(num1, num2, 0, fr);},
        2,8
    };
}
Instruction SBB_8_reg(uint8_t& num1, uint8_t& num2, FlagRegister& fr) {
    return Instruction{ 
        [&](){Primitive::sub_8_bit(num1, num2, fr.get_flag(Flag::CARRY), fr);}, 
        1,4 
    };
}
Instruction SBB_8_mem(uint8_t& num1, MMU& mmu, uint16_t addr, FlagRegister& fr) {
    return Instruction{
        [&](){
            uint8_t num2 = mmu.read(addr);
            Primitive::sub_8_bit(num1, num2, fr.get_flag(Flag::CARRY), fr);
            mmu.write(addr, num1);
        },
        1,8 
    };
}
Instruction SBB_8_imm(uint8_t& num1, uint8_t num2, FlagRegister& fr) {
    return Instruction{ 
        [&](){Primitive::sub_8_bit(num1, num2, fr.get_flag(Flag::CARRY), fr);},
        2,8
    };
}
Instruction DEC_8(uint8_t& num, FlagRegister& fr){
    return Instruction{ 
        [&](){Primitive::dec_8_bit(num, fr);},
        1,4 
    };
}
Instruction DEC_8_mem(MMU& mmu, uint16_t addr, FlagRegister& fr){
    return Instruction{ 
        [&](){
            uint8_t num = mmu.read(addr);
            Primitive::dec_8_bit(num, fr), 
            mmu.write(addr,num);
        },
        1,12 
    };        
}
Instruction DEC_16(uint8_t& hi, uint8_t& lo) {
    return Instruction{
        [&](){
            uint16_t pair = Arithmetic::pair(hi, lo);
            pair--;
            hi = pair >> 8;
            lo = pair & 0xff;
        }, 
        1, 8
    };
}      

//------------------LOGICAL------------------//
Instruction AND_8_reg(uint8_t& num1, uint8_t& num2, FlagRegister& fr) {
    return Instruction{
        [&](){Primitive::and_8_bit(num1, num2, fr);},
        1,4
    };
}
Instruction AND_8_mem(uint8_t& num1, MMU& mmu, uint16_t addr, FlagRegister& fr){
    return Instruction{
        [&](){
            uint8_t num2 = mmu.read(addr);
            Primitive::and_8_bit(num1, num2, fr);
            mmu.write(addr, num1);
        },
        1,8
    };
}
Instruction AND_8_imm(uint8_t& num1, uint8_t num2, FlagRegister& fr) {
    return Instruction{
        [&](){Primitive::and_8_bit(num1, num2, fr);},
        2,8
    };
}

Instruction OR_8_reg(uint8_t& num1, uint8_t& num2, FlagRegister& fr) {
    return Instruction{
        [&](){Primitive::or_8_bit(num1, num2, fr);},
        1,4
    };
}
Instruction OR_8_mem(uint8_t& num1, MMU& mmu, uint16_t addr, FlagRegister& fr){
    return Instruction{
        [&](){
            uint8_t num2 = mmu.read(addr);
            Primitive::or_8_bit(num1, num2, fr);
            mmu.write(addr, num1);
        },
        1,8
    };
}
Instruction OR_8_imm(uint8_t& num1, uint8_t num2, FlagRegister& fr) {
    return Instruction{
        [&](){Primitive::or_8_bit(num1, num2, fr);},
        2,8
    };
}

Instruction XOR_8_reg(uint8_t& num1, uint8_t& num2, FlagRegister& fr) {
    return Instruction{
        [&](){Primitive::xor_8_bit(num1, num2, fr);},
        1,4
    };
}
Instruction XOR_8_mem(uint8_t& num1, MMU& mmu, uint16_t addr, FlagRegister& fr){
    return Instruction{
        [&](){
            uint8_t num2 = mmu.read(addr);
            Primitive::xor_8_bit(num1, num2, fr);
            mmu.write(addr, num1);
        },
        1,8
    };
}
Instruction XOR_8_imm(uint8_t& num1, uint8_t num2, FlagRegister& fr) {
    return Instruction{
        [&](){Primitive::xor_8_bit(num1, num2, fr);},
        2,8
    };
}

Instruction CP_8_reg(uint8_t& num1, uint8_t& num2, FlagRegister& fr) {
    return Instruction{
        [&](){Primitive::compare_8_bit(num1, num2, fr);},
        1,4
    };
}
Instruction CP_8_mem(uint8_t& num1, MMU& mmu, uint16_t addr, FlagRegister& fr){
    return Instruction{
        [&](){
            uint8_t num2 = mmu.read(addr);
            Primitive::compare_8_bit(num1, num2, fr);
            mmu.write(addr, num1);
        },
        1,8
    };
}
Instruction CP_8_imm(uint8_t& num1, uint8_t num2, FlagRegister& fr) {
    return Instruction{
        [&](){Primitive::compare_8_bit(num1, num2, fr);},
        2,8
    };
}
}   //Operation