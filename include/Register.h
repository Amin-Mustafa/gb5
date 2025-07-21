#ifndef REGISTER_H
#define REGISTER_H

#include <cstdint>

class MMU;

enum class Flag {
    CARRY = 4, HALF_CARRY, NEGATIVE, ZERO
};

constexpr uint8_t flag_state(bool z, bool n, bool h, bool c) {
    return z << 7 | n << 6 | h << 5 | c << 4;
}

class Register {
public:
    virtual uint8_t get() const = 0;
    virtual void set(uint8_t val) = 0;
    virtual ~Register()=default;
};

class DataRegister : public Register {
private: 
    uint8_t data;
public: 
    DataRegister(const uint8_t& val) : data{val} {}

    uint8_t get() const override {return data;}
    void set(uint8_t val) override {data = val;}
    DataRegister& operator=(uint8_t val) {
        data = val;
        return *this;
    }
};

class MemRegister : public Register {
private: 
    MMU& mem;
    uint8_t& hi;
    uint8_t& lo;
public:    
    MemRegister(MMU& memory, uint8_t& high, uint8_t& low)    
        :mem{memory}, hi{high}, lo{low} {}

    uint8_t get() const override;
    void set(uint8_t val) override;
};

class FlagRegister : public Register {
private:
    uint8_t reg;
public:
    FlagRegister(const uint8_t& state):
        reg{state} {}
    
    uint8_t get() const override {return reg;}
    void set(uint8_t val) override {reg = val;}

    void set_flag(Flag fl, bool val) {
        reg = (reg & ~((uint8_t)1 << (int)fl)) | ((uint8_t)val << (int)fl);
    }
    bool get_flag(Flag fl) const {
        return (reg >> (int)fl) & (uint8_t)1;
    }
    uint8_t get_state() const {
        return reg;
    }
};

class RegisterPair {
private: 
    DataRegister& hi;
    DataRegister& lo;
public:    
    uint16_t get() const {
        return ( static_cast<uint16_t>(hi.get()) << 8 ) | lo.get();
    }       
    void set(uint16_t val) {
        hi.set(val >> 8);
        lo.set(val & 0xff);
    }
    
};

#endif