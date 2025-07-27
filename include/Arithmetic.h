#ifndef ARITHMETIC_H
#define ARITHMETIC_H

#include <cstdint>

namespace Arithmetic {
inline bool half_carry_add(uint8_t num1, uint8_t num2) {
    return (((num1 & 0x0F) + (num2 & 0x0F)) & 0x10) == 0x10;
}

inline bool half_carry_add(uint16_t num1, uint16_t num2) {
    return (((num1 & 0x00FF) + (num2 & 0x00FF)) & 0x0100) == 0x0100;
}

inline bool half_carry_sub(uint8_t num1, uint8_t num2) {
    return (int)(num1 & 0x0F) - (int)(num2 & 0x0F) < 0;
}

inline bool half_carry_sub(uint16_t num1, uint16_t num2) {
    return (int)(num1 & 0x00FF) - (int)(num2 & 0x00FF) < 0;
}

inline uint16_t pair(uint8_t hi, uint8_t lo) {
    return (static_cast<uint16_t>(hi) << 8) | lo;
}
inline uint8_t rot_left_circ(uint8_t num, bool& carry) {
    carry = num >> 7;
    return (num << 1) | (num >> 7);
}
inline uint8_t rot_left(uint8_t num, bool& carry) {
    bool old_carry = carry;
    carry = num >> 7;
    return (num << 1) | old_carry;
}
inline uint8_t rot_right_circ(uint8_t num, bool& carry) {
    carry = num & 1;
    return (num >> 1) | (num << 7);
}
inline uint8_t rot_right(uint8_t num, bool& carry){
    bool old_carry = carry;
    carry = num & 1;
    return (num >> 1) | (old_carry << 7);
}
inline uint8_t shift_left_arithmetic(uint8_t num, bool& carry) {
    carry = num >> 7;
    return num << 1;
}
inline uint8_t shift_right_arithmetic(uint8_t num, bool& carry) {
    uint8_t msb = num & 0x80;
    carry = num & 1;
    return (num >> 1) | msb;
}
inline uint8_t shift_right_logical(uint8_t num, bool& carry){
    carry = num & 1;
    return num >> 1;
}
}

#endif