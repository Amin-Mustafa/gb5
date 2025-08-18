#ifndef INTERRUPTHANDLER_H
#define INTERRUPTHANDLER_H

#include <cstdint>
#include "MappedRegister.h"

class MMU;
class Instruction;
class CPU;

class InterruptHandler {
private:
    MappedRegister irq; 
    MappedRegister ie;
public:
    enum Interrupt {
        VBLANK = 0, LCD, SERIAL, TIMER, JOYPAD, num_interrupts
    };

    InterruptHandler(MMU& mem);
    bool active();
    bool requested(Interrupt kind);
    void request(Interrupt kind);
    void clear(Interrupt kind);
    uint16_t service_addr(Interrupt kind);
};

#endif