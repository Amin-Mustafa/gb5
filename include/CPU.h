#ifndef CPU_H
#define CPU_H

#include <cstdint>
#include <array>
#include <memory>
#include <string>
#include <iostream>
#include "Register.h"
#include "Decoder.h"

class MMU;
class Instruction;

class CPU {
public:
    CPU(MMU& memory);

    void tick() {(this->*current_state)();}
    void print_state();
    void log_state(std::ostream& os);

    //flags
    FlagRegister F;
    //program counter and stack pointer
    uint16_t pc;    
    StackPointer sp;
    //Data registers
    DataRegister A, B, C, D, E, H, L;
    MemRegister M;

    //memory
    MMU& memory;
    uint8_t read_memory(uint16_t addr);
    void write_memory(uint16_t addr, uint8_t val);
    
private:
    using StateFunction = void (CPU::*)();  //pointer to state function

    int cycles;
    bool int_enable;

    //CPU states
    StateFunction current_state;
    void fetch_and_execute();

    
    //internal functions
    std::unique_ptr<Decoder> decoder;

    void jump(uint16_t addr);
    void pc_return();
    void push_to_stack(uint16_t num);
    void pop_from_stack(uint8_t& num_hi, uint8_t& num_lo);
    void pop_from_stack(uint16_t& num);
    static bool no_cond(const FlagRegister&) { return true; }

public:
    //CPU-control opcodes
    Instruction JR(const Register8& offset, FlagRegister::ConditionCheck = no_cond);
    Instruction JP(const Register16& destination, FlagRegister::ConditionCheck = no_cond);
    Instruction JPHL();
    Instruction CALL(const Register16& destination, FlagRegister::ConditionCheck = no_cond);
    Instruction RET();
    Instruction RETI();
    Instruction RET_IF(FlagRegister::ConditionCheck);
    Instruction RST(uint8_t destination);
    Instruction PUSH(RegisterPair& rp);
    Instruction POP(RegisterPair& rp);
    Instruction DI();
    Instruction EI();
};

#endif
