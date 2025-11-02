#ifndef ARITHMETIC_H
#define ARITHMETIC_H

#include <cstdint>

namespace Arithmetic {
inline bool half_carry_add_8(uint8_t a, uint8_t b, bool carry = 0) {
    return ((a & 0xF) + (b & 0xF) + carry) > 0xF;
}

inline bool half_carry_add_16(uint16_t a, uint16_t b) {
    return ((a & 0x0FFF) + (b & 0x0FFF)) & 0x1000;
}

inline bool half_carry_sub_8(uint8_t a, uint8_t b, bool carry = 0) {
    return ((a & 0xF) - (b & 0xF) - carry) < 0;
}

inline bool half_carry_sub_16(uint16_t a, uint16_t b) {
    return (a & 0x0FFF) < (b & 0x0FFF);
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
    return ((num >> 1) | (old_carry << 7));
}
inline uint8_t shift_left_arithmetic(uint8_t num, bool& carry) {
    carry = num >> 7;
    return num << 1;
}
inline uint8_t shift_right_arithmetic(uint8_t num, bool& carry) {
    uint8_t msb = num & 0x80;
    carry = num & 1;
    return ((num >> 1) | msb);
}
inline uint8_t shift_right_logical(uint8_t num, bool& carry){
    carry = num & 1;
    return num >> 1;
}
inline uint8_t swap_nibs(uint8_t num){
    return ( (num & 0x0F) << 4 | (num & 0xF0) >> 4 );
}
inline uint8_t bit_set(uint8_t num, uint8_t bit) {
    return (num | ((uint8_t)1 << bit));
}
inline uint8_t bit_clear(uint8_t num, uint8_t bit) {
    return (num & ~((uint8_t)1 << bit));
}
inline bool bit_check(uint8_t num, uint8_t bit) {
    return ((num >> bit) & 1);
}
}

#endif