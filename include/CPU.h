#ifndef CPU_H
#define CPU_H

#include <cstdint>
#include <array>
#include <memory>
#include <string>
#include "Register.h"
#include "Decoder.h"

class MMU;
class Instruction;

class CPU {
public:
    CPU(MMU& memory);

    void tick() {(this->*current_state)();}
    void print_state();

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

    //internal functions
    std::unique_ptr<Decoder> decoder;
    
    void jump(uint16_t addr);
    void pc_return();
    void push_to_stack(uint16_t num);
    void pop_from_stack(uint8_t& num_hi, uint8_t& num_lo);
    void pop_from_stack(uint16_t& num);

    //CPU states
    StateFunction current_state;
    void fetch_and_execute();

public:
    //CPU-control opcodes
    Instruction JR(uint8_t offset, bool condition);
    Instruction JP(uint16_t destination, bool condition);
    Instruction CALL(uint16_t destination, bool condition);
    Instruction RET();
    Instruction RETI();
    Instruction RET_IF(bool condition);
    Instruction RST(uint8_t destination);
    Instruction PUSH(uint8_t num_hi, uint8_t num_lo);
    Instruction POP(uint8_t& num_hi, uint8_t& num_lo);
    Instruction DI();
    Instruction EI();
};

#endif
