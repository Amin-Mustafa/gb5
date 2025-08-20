#ifndef SERIALPORT_H
#define SERIALPORT_H

#include <vector>
#include <cstdint>
#include "MemoryRegion.h"

class MMU;

class SerialPort {
//TODO: make this functional with master and slave gameboys
//for now this is only for serial debugging e.g. Blargg tests
public:
    SerialPort(MMU& mem);
    uint8_t serial_read(uint16_t addr);
    void serial_write(uint16_t addr, uint8_t val); 
    std::vector<uint8_t> destination_buffer;
private: 
    //Serial port maps 0xFF01 and 0xFF02 
    uint8_t serial[2];      
    //serial[0]: data to be sent
    //serial[1]: serial control
    MemoryRegion region;
};

#endif