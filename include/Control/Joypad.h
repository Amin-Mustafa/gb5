#ifndef JOYPAD_H
#define JOYPAD_H

#include "Memory/IO.h"

class MMU;
class InterruptController;
class InputHandler;
class Bus;

class JoyPad : public IO {
private:
    //memory-mapped joypad register
    uint8_t data;   

    InterruptController& ic;

    InputHandler* ih;   //SDL input handler to capture current key state

    uint8_t dpad_state;
    uint8_t button_state;
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

    uint8_t read(uint16_t addr) override;
    void write(uint16_t addr, uint8_t val) override; 
};

#endif