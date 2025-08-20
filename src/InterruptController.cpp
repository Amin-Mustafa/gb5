#include "../include/InterruptController.h"
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

bool InterruptController::requested(Interrupt kind) {
    return Arithmetic::bit_check(irq.get(), kind);
}

void InterruptController::clear(Interrupt kind) {
    irq.set(Arithmetic::bit_clear(irq.get(), kind));
}

void InterruptController::request(Interrupt kind) {
    irq.set(Arithmetic::bit_set(irq.get(), kind));
}

uint16_t InterruptController::service_addr(Interrupt kind) {
    return 0x40 + (kind * 0x08);
}