#include "../include/Timer.h" 
#include "../include/Memory/MMU.h"
#include "../include/Memory/InterruptController.h"  
#include <iostream>

enum TimerAddress {
    DIV  = 0xFF04,
    TIMA = 0xFF05,
    TMA  = 0xFF06,
    TAC  = 0xFF07,
};

constexpr unsigned long CLOCK_SPEED = 4194304;

Timer::Timer(MMU& mmu, InterruptController& int_controller)
    :div{0x18},
     counter{0x00},
     modulo{0x00},
     control{0xF8},
     cycles{0},
     region{this, START, END},
     ic{int_controller}
    {
        mmu.add_region(&region);
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

unsigned long Timer::frequency() const {
    uint8_t freq = control & 0x03;
    switch(freq) {
        case 0: return 1024;
        case 1: return 16;
        case 2: return 64;
        case 3: return 256;
    }
}

void Timer::tick() {
    div++;  // Keep incrementing every cycle
    
    // Check falling edges based on selected frequency
    bool bit = div & frequency();

    if(enabled() && increment_trigger.falling_edge(bit)) {
        counter++;
        if(counter == 0) {
            counter = modulo;
            ic.request(Interrupt::TIMER);
            std::cout << "TIMER INTERRUPT REQUESTED\n";
        }
    }
}