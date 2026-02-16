#ifndef SERIALPORT_H
#define SERIALPORT_H

#include <vector>
#include <cstdint>
#include "Memory/IO.h"

class MMU;

class SerialPort : public IO {
//TODO: make this functional with master and slave gameboys
//for now this is only for serial debugging e.g. Blargg tests
public:
    static constexpr uint16_t START = 0xFF01;
    static constexpr uint16_t END   = 0xFF02;
    SerialPort(MMU& mem);
    uint8_t read(uint16_t addr) override;
    void write(uint16_t addr, uint8_t val) override; 
private: 
    //Serial port maps 0xFF01 and 0xFF02 
    uint8_t serial[2];      
    //serial[0]: data to be sent
    //serial[1]: serial control
};

#endif