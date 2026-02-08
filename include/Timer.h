#ifndef TIMER_H
#define TIMER_H

#include <cstdint>
#include "Memory/MemoryRegion.h"
#include "EdgeDetector.h"

class MMU;
class InterruptController;
class Bus;

class Timer {
private:
    uint16_t div;
    uint8_t counter;
    uint8_t modulo;
    uint8_t control;

    unsigned long cycles;
    EdgeDetector increment_trigger;

    MemoryRegion region;
    InterruptController& ic;

public:
    static constexpr uint16_t START = 0xFF04;
    static constexpr uint16_t END   = 0xFF07;

    Timer(Bus& bus, MMU& mmu, InterruptController& int_controller);
    void tick();

    uint8_t ext_read(uint16_t addr);
    void ext_write(uint16_t addr, uint8_t val);

    unsigned long frequency() const;
    bool enabled() const {
        return control & 0x04;
    }
};

#endif