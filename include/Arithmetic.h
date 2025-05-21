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
}

#endif