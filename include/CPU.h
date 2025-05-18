#ifndef CPU_H
#define CPU_H

#include <stdint.h>
#include <variant>
#include "Register.h"
#include <string>

namespace SM83 {
class Instruction;
class MMU;

class CPU {
public:
    using StateFunction = void (CPU::*)();  //pointer to state function
    //Data registers
    uint8_t A, B, C, D, E, H, L;
    uint8_t* regs[7] = {&A, &B, &C, &D, &E, &H, &L};
    FlagRegister F;
    uint16_t pc;
    uint16_t sp;
    int cycles;

    //memory
    MMU& memory;

    //internal function 
    Instruction decode(uint8_t opcode);
    Instruction jr(uint8_t offset, bool condition, std::string name);

    //CPU states
    StateFunction current_state;
    void fetch_and_execute();

public:
    CPU(MMU& memory): 
        memory{memory}, current_state{&fetch_and_execute} {}

    void tick() {(this->*current_state)();}
};
}

#endif
