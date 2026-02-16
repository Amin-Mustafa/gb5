#include "Memory/InterruptController.h"
#include "Memory/MMU.h"
#include "Arithmetic.h"

InterruptController::InterruptController(MMU& mmu)
    {
        mmu.map_io_register(IF, &irq);
        mmu.map_io_register(IE, &ie);
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

    throw std::runtime_error("BAD INTERRUPT");
}