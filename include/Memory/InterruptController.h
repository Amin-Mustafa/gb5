#ifndef INTERRUPTCONTROLLER_H
#define INTERRUPTCONTROLLER_H

#include <cstdint>
#include <array>
#include "Memory/IO.h"

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

    static constexpr uint16_t IF = 0xFF0F;
    static constexpr uint16_t IE = 0xFFFF;
    
public:
    InterruptController(MMU& mem);
    bool active();
    void request(Interrupt kind);
    void clear(Interrupt kind);

    Interrupt pending();
};

#endif