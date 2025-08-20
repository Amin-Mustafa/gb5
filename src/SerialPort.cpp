#include <iostream>
#include "../include/SerialPort.h"
#include "../include/MMU.h"
#include "../include/MemoryRegion.h"

uint8_t SerialPort::serial_read(uint16_t addr) {
    switch(addr) {
        case 0xFF01: return serial[0];
        case 0xFF02: return serial[1];
        default: return 0xFF;
    }
}

void SerialPort::serial_write(uint16_t addr, uint8_t val){
    switch(addr) {
        case 0xFF01: 
            serial[0] = val;
            break;
        case 0xFF02: 
            serial[1] = val;
            //send data to destination if transfer is enabled (bit 7 set)
            if(val & 0x80) {
                destination_buffer.push_back(serial[0]); 
                serial[1] &= ~0x80; //clear bit 7 (transfer complete)
            }   
            break;
        default: break;
    }
}

SerialPort::SerialPort(MMU& mmu)
    :region {
        mmu,
        0xFF01, 0xFF02,
        [this](uint16_t addr){return serial_read(addr);},
        [this](uint16_t addr, uint8_t val) { serial_write(addr,val); }
    } {}