#ifndef TIMER_H
#define TIMER_H

#include <cstdint>
#include "EdgeDetector.h"
#include "Memory/IO.h"

class MMU;
class InterruptController;
class Bus;

class Timer : public IO {
private:
    uint16_t div;
    uint8_t counter;
    uint8_t modulo;
    uint8_t control;

    unsigned long cycles;
    EdgeDetector increment_trigger;

    InterruptController& ic;

public:
    static constexpr uint16_t START = 0xFF04;
    static constexpr uint16_t END   = 0xFF07;

    Timer(Bus& bus, MMU& mmu, InterruptController& int_controller);
    void tick();

    uint8_t read(uint16_t addr) override;
    void write(uint16_t addr, uint8_t val) override;

    unsigned long frequency() const;
    bool enabled() const {
        return control & 0x04;
    }
};

#endif