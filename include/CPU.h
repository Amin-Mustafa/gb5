#ifndef CPU_H
#define CPU_H

#include <stdint.h>

namespace SM83 {
class Instruction;
class MMU;

class CPU {
private:
    using StateFunction = void (CPU::*)();  //pointer to state function
    //Data registers
    uint8_t A, B, C, D, E, H, L;
    uint16_t _pc;
    uint16_t _sp;
    int _cycles;

    //CPU states
    StateFunction current_state;
    void fetch_and_execute();

public:
    CPU(): 
        current_state{&fetch_and_execute} {}
    void tick() {(this->*current_state)();}
};
}

#endif
