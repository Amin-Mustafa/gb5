#ifndef IO_H
#define IO_H

#include <cstdint>

struct IO {
//abstract IO register
public:
    virtual uint8_t read(uint16_t addr) = 0;
    virtual void write(uint16_t addr, uint8_t val) = 0;
};

#endif