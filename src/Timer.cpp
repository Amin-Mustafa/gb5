#include "Timer.h" 
#include "Memory/MMU.h"
#include "Memory/InterruptController.h"  
#include "Memory/Bus.h"
#include <iostream>

enum TimerAddress {
    DIV  = 0xFF04,
    TIMA = 0xFF05,
    TMA  = 0xFF06,
    TAC  = 0xFF07,
};

constexpr unsigned long CLOCK_SPEED = 4194304;
uint8_t frequency_bit(uint8_t control_reg);

Timer::Timer(Bus& bus, MMU& mmu, InterruptController& int_controller)
    :div{0x18},
     counter{0x00},
     modulo{0x00},
     control{0xF8},
     increment_trigger{},
     region{this, START, END},
     ic{int_controller}
    {
        mmu.add_region(&region);
        bus.connect(*this);
    }
    
uint8_t Timer::ext_read(uint16_t addr) {
    switch(addr) {
        case DIV : return static_cast<uint8_t>(div >> 8);
        case TIMA: return counter;
        case TMA : return modulo;
        case TAC : return control;

        default: return 0xFF;
    }
}

void Timer::ext_write(uint16_t addr, uint8_t val) {
    switch(addr) {
        case DIV : div = 0;         break;
        case TIMA: counter = val;   break;
        case TMA : modulo = val;    break;
        case TAC : control = val;   break; 

        default: break;
    }
}

void Timer::tick() {
    uint16_t old_div = div;  // Keep incrementing every cycle
    div++;

    bool old_inc = (old_div >> frequency_bit(control)) & 1;
    bool new_inc = (div >> frequency_bit(control)) & 1;

    if( enabled() && (old_inc && !new_inc) ) {
        counter++;
        if(counter == 0) {
            counter = modulo;
            ic.request(Interrupt::TIMER);
        }
    }
}

uint8_t frequency_bit(uint8_t control_reg) {
    switch(control_reg & 0x03) {
        case 0 : return 9;
        case 1 : return 3;
        case 2 : return 5;
        case 3 : return 7;
        default: return 0;
    }
}