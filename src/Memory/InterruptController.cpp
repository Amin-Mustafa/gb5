#include "../include/Memory/InterruptController.h"
#include "../include/Arithmetic.h"
#include "../include/CPU.h"
#include "../include/Instruction.h"

InterruptController::InterruptController(MMU& mem)
    :irq{mem, 0xFF0F}, ie{mem, 0xFFFF} 
    {
        irq.set(0xE1);
        ie.set(0x00);
    }

bool InterruptController::active() {
    return (irq.get() & ie.get() & 0x1F);
}

void InterruptController::clear(Interrupt kind) {
    irq.set( Arithmetic::bit_clear(irq.get(), static_cast<uint8_t>(kind)) );
}

void InterruptController::request(Interrupt kind) {
    irq.set( Arithmetic::bit_set(irq.get(), static_cast<uint8_t>(kind)) );
}

Interrupt InterruptController::pending() {
    using Arithmetic::bit_check;

    bool requested;
    bool enabled;
    uint8_t index;

    for(Interrupt src : sources) {
        index = static_cast<uint8_t>(src);
        requested = bit_check(irq.get(), index);
        enabled = bit_check(ie.get(), index);
        if(requested && enabled) {
            return src;
        }
    }

    return Interrupt::NULL_INTERRUPT;
}