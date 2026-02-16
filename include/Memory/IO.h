#ifndef IO_H
#define IO_H

#include <cstdint>

struct IO {
//abstract IO register
public:
    virtual uint8_t read(uint16_t addr) = 0;
    virtual void write(uint16_t addr, uint8_t val) = 0;
};

struct MappedRegister : IO {
private:
    uint8_t data;
public:
    uint8_t get() const {return data;}
    void set(uint8_t val) {data = val;}

    uint8_t read(uint16_t addr) override {return get();}
    void write(uint16_t addr, uint8_t val) override {set(val);}
};

#endif