#ifndef DMACONTROLLER_H
#define DMACONTROLLER_H

#include <cstdint>
#include <iostream>

constexpr unsigned int DMA_CYCLES = 160;

class DmaController {
private:
    uint8_t cycles; 
    bool on;
    uint16_t start_addr;
public:

    void start(uint16_t addr) {
        cycles = 0;
        start_addr = addr;
        on = true;
    }
    void tick() {
        if(on) {
            cycles++;
            if(cycles == DMA_CYCLES) {
                on = false;
            }
        }
    }
    bool active() const {
        return on;
    }
    uint8_t offset() const {
        //offset from DMA start address
        return cycles;
    }
    uint8_t start_address() const {
        return start_addr;
    }
};

#endif