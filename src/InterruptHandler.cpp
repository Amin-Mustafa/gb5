#include "../include/InterruptHandler.h"
#include "../include/Arithmetic.h"
#include "../include/CPU.h"
#include "../include/Instruction.h"

InterruptHandler::InterruptHandler(MMU& mem)
    :irq{mem, 0xFF0F}, ie{mem, 0xFFFF} 
    {
        irq.set(0xE1);
        ie.set(0x00);
    }

bool InterruptHandler::active() {
    return (irq.get() & ie.get() & 0x1F);
}

bool InterruptHandler::requested(Interrupt kind) {
    return Arithmetic::bit_check(irq.get(), kind);
}

void InterruptHandler::clear(Interrupt kind) {
    irq.set(Arithmetic::bit_clear(irq.get(), kind));
}

void InterruptHandler::request(Interrupt kind) {
    irq.set(Arithmetic::bit_set(irq.get(), kind));
}

uint16_t InterruptHandler::service_addr(Interrupt kind) {
    return 0x40 + (kind * 0x08);
}