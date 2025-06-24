#ifndef REGISTER_H
#define REGISTER_H

#include <cstdint>

namespace SM83 {
enum class Flag {
    CARRY = 4, HALF_CARRY, NEGATIVE, ZERO
};

constexpr uint8_t flag_state(bool z, bool n, bool h, bool c) {
    return z << 7 | n << 6 | h << 5 | c << 4;
}

class FlagRegister {
public:
    void set_flag(Flag fl, bool val) {
        reg = (reg & ~((uint8_t)1 << (int)fl)) | ((uint8_t)val << (int)fl);
    }
    bool get_flag(Flag fl) const {
        return (reg >> (int)fl) & (uint8_t)1;
    }
    uint8_t get_state() const {
        return reg;
    }
    //overload assignment to uint8_t for POP operation
    FlagRegister& operator=(uint8_t val) {
        reg = val;
        return *this;
    }
    FlagRegister(const uint8_t& state):
        reg{state} {}
private:
    uint8_t reg;
};
}

#endif