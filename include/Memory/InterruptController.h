#ifndef INTERRUPTCONTROLLER_H
#define INTERRUPTCONTROLLER_H

#include <cstdint>
#include <array>
#include "MappedRegister.h"

class MMU;
class Instruction;
class CPU;


enum class Interrupt : uint8_t {
    VBLANK = 0, LCD, TIMER, SERIAL, JOYPAD
};

class InterruptController {
public:
    MappedRegister irq; 
    MappedRegister ie;
    static constexpr std::array<Interrupt, 5> sources = {
        Interrupt::VBLANK,
        Interrupt::LCD,
        Interrupt::TIMER,
        Interrupt::SERIAL,
        Interrupt::JOYPAD
    };
    
public:
    InterruptController(MMU& mem);
    bool active();
    void request(Interrupt kind);
    void clear(Interrupt kind);

    Interrupt pending();
};

#endif