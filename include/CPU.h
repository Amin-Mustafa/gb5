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
    FlagRegister F;
    uint16_t pc;
    uint16_t sp;
    int cycles;

    //memory
    MMU& memory;

    //internal functions
    Instruction decode(uint8_t opcode);
    //stack ops
    void push_to_stack(uint16_t num);
    void pop_from_stack(uint8_t& num_hi, uint8_t& num_lo);
    void pop_from_stack(uint16_t& num);

    //CPU states
    StateFunction current_state;
    void fetch_and_execute();

public:
    CPU(MMU& memory): 
        memory{memory}, current_state{&fetch_and_execute} {}

    void tick() {(this->*current_state)();}

private:
    //CPU-control opcodes
    Instruction JR(uint8_t offset, bool condition);
    Instruction JP(uint16_t destination, bool condition);
    Instruction CALL(uint16_t destination, bool condition);
    Instruction RET(bool condition);
    Instruction RST(uint8_t destination);
    Instruction PUSH(uint8_t num_hi, uint8_t num_lo);
    Instruction POP(uint8_t& num_hi, uint8_t& num_lo);
};
}

#endif
