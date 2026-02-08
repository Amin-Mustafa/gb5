#ifndef JOYPAD_H
#define JOYPAD_H

#include "../Memory/MemoryRegion.h"

class MMU;
class InterruptController;
class InputHandler;
class Bus;

class JoyPad {
private:
    //memory-mapped joypad register
    uint8_t data;   
    MemoryRegion region;

    InterruptController& ic;

    InputHandler* ih;   //SDL input handler to capture current key state
public:
    static constexpr uint16_t ADDRESS = 0xFF00;
    JoyPad(Bus& bus, MMU& mmu, InterruptController& ic);

    void connect_input_handler(InputHandler* input_handler) {
        ih = input_handler;
    }

    void read_input();

    bool buttons_enabled() {
        return !((data >> 5) & (uint8_t)1);
    }
    bool dpad_enabled() {
        return !((data >> 4) & (uint8_t)1);
    }

    uint8_t ext_read(uint16_t addr);
    void ext_write(uint16_t addr, uint8_t val); 
};

#endif