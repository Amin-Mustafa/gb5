#ifndef REGISTER_H
#define REGISTER_H

#include <cstdint>

class CPU;

enum class Flag {
    CARRY = 4, HALF_CARRY, NEGATIVE, ZERO
};

constexpr uint8_t flag_state(bool z, bool n, bool h, bool c) {
    return z << 7 | n << 6 | h << 5 | c << 4;
}

class Register8 {
public:
    virtual uint8_t get() const = 0;
    virtual void set(uint8_t val) = 0;
    virtual ~Register8()=default;
};

class DataRegister : public Register8 {
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
    //treat a data register just like a number
    operator uint8_t() const { return get(); } 
};

class MemRegister : public Register8 {
private: 
    CPU& cpu;
    Register8& hi;
    Register8& lo;
public:    
    MemRegister(CPU& cpu, Register8& high, Register8& low)    
        :cpu{cpu}, hi{high}, lo{low} {}

    uint8_t get() const override;
    void set(uint8_t val) override;
};

class Immediate8 : public Register8 {
private: 
    CPU& cpu;
public:    
    Immediate8(CPU& cpu) :cpu{cpu} {}

    uint8_t get() const override;
    void set(uint8_t) {/* nothing */}; //cannot set immediate value
};

class HighMemory : public Register8 {
private:
    CPU& cpu;
public:
    HighMemory(CPU& cpu) :cpu{cpu} {}
    uint8_t get() const override;
    void set(uint8_t);
};

class FlagRegister : public Register8 {
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

class Register16 {
public:
    virtual uint16_t get() const = 0;
    virtual void set(uint16_t val) = 0;
    virtual ~Register16()=default;
};

class RegisterPair : public Register16 {
private: 
    Register8& hi;
    Register8& lo;
public: 
    RegisterPair(Register8& high, Register8& low) 
        :hi{high}, lo{low} {}

    uint16_t get() const {
        return ( static_cast<uint16_t>(hi.get()) << 8 ) | lo.get();
    }       
    void set(uint16_t val) {
        hi.set(val >> 8);
        lo.set(val & 0xff);
    }
};

class Immediate16 : public Register16 {
private: 
    CPU& cpu;
public:    
    Immediate16(CPU& cpu) :cpu{cpu} {}
    uint16_t get() const;
    void set(uint16_t) {/* nothing */};
};

class StackPointer : public Register16 {
private:
    uint16_t val;
public:
    StackPointer(uint16_t num) : val{num} {}
    uint16_t get() const {return val;}
    void set(uint16_t num) {val = num;}
    StackPointer& operator=(uint16_t num) {
        set(num);
        return *this;
    }
    operator uint8_t() const { return get(); } 
};

#endif