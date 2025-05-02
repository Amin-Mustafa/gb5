#include "../include/Instruction.h"
#include "../include/CPU.h"


//helper to quickly obtain the Operand as a uint8_t or a uint16_t
template<typename T> 
inline T& get_ref(SM83::Instruction::Operand& operand) {
    return std::get<std::reference_wrapper<T>>(operand).get();
}

inline void bit_set(uint8_t& number, int n) {
    number = number | ((int)1 << n);
}
inline void bit_clear(uint8_t& number, int n) {
    number = number & ~((int)1 << n);
}
inline void bit_toggle(uint8_t& number, int n) {
    number = number ^ ((int)1 << n);
}
inline bool bit_check(uint8_t& number, int n) {
    return (number >> n) & (int)1;
}
inline void bit_set_to(uint8_t& number, int n, bool x) {
    number = (number & ~((int)1 << n)) | ((int)x << n);
}
static constexpr bool half_carry_add(uint8_t first_num, uint8_t second_num)
{
    return (((first_num & 0x0F) + (second_num & 0x0F)) & 0x10) == 0x10;
}

static constexpr bool half_carry_add(uint16_t first_num, uint16_t second_num)
{
    return (((first_num & 0x00FF) + (second_num & 0x00FF)) & 0x0100) == 0x0100;
}

static constexpr bool half_carry_sub(uint8_t first_num, uint8_t second_num)
{
    return (int)(first_num & 0x0F) - (int)(second_num & 0x0F) < 0;
}

static constexpr bool half_carry_sub(uint16_t first_num, uint16_t second_num)
{
    return (int)(first_num & 0x00FF) - (int)(second_num & 0x00FF) < 0;
}

enum FLAGS {
    CARRY = 4, HALF_CARRY = 5, NEGATIVE = 6, ZERO = 7
};

namespace SM83 {

typedef Instruction::Args Args;
//Basic operations

void nop(Args& args){

}

void load8(Args& args) {
    get_ref<uint8_t>(args[0]) = get_ref<uint8_t>(args[1]);
}

void load16(Args& args) {
    get_ref<uint16_t>(args[0]) = get_ref<uint16_t>(args[1]);
}

void load_pair(Args& args) {
    get_ref<uint8_t>(args[0]) = get_ref<uint8_t>(args[2]);
    get_ref<uint8_t>(args[1]) = get_ref<uint8_t>(args[3]);
}

void inc8(Args& args){
    uint8_t& num = get_ref<uint8_t>(args[0]);
    uint8_t& flags = get_ref<uint8_t>(args[1]);
    bit_set_to(flags, ZERO, static_cast<uint8_t>(num + 1) == 0);
    bit_clear(flags, NEGATIVE);
    bit_set_to(flags, HALF_CARRY, half_carry_add(num, 1));
    num += 1;
}

void dec8(Args& args){
    uint8_t& num = get_ref<uint8_t>(args[0]);
    uint8_t& flags = get_ref<uint8_t>(args[1]);
    bit_set_to(flags, ZERO, static_cast<uint8_t>(num - 1) == 0);
    bit_set(flags, NEGATIVE);
    bit_set_to(flags, HALF_CARRY, half_carry_sub(num, 1));
    num -= 1;
}

void inc16(Args& args) {
    get_ref<uint16_t>(args[0]) += 1;
}

}