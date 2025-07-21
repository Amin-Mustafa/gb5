#ifndef CPU_H
#define CPU_H

#include <cstdint>
#include <array>
#include <string>
#include "Register.h"
#include "Instruction.h"

class Instruction;
class MMU;

class CPU {
public:
    CPU(MMU& memory);

    void tick() {(this->*current_state)();}
    void print_state();

    //flags
    FlagRegister F;
    //program counter and stack pointer
    uint16_t pc;
    uint16_t sp;
    //Data registers
    DataRegister A, B, C, D, E, H, L;
    MemRegister M;
    
    int cycles;
    bool int_enable;

    //memory
    MMU& memory;
    uint8_t read_memory(uint16_t addr) {
        cycles += 4;    //CPU memory access costs 4 cycles 
        return memory.read(addr);
     }
    void write_memory(uint16_t addr, uint8_t val) { 
        cycles += 4;   
        memory.write(addr, val); 
    }
    
private:
    using StateFunction = void (CPU::*)();  //pointer to state function

    //internal functions
    Instruction decode(uint8_t opcode);
    std::array<Instruction, 256> instruction_table;
    void init_instruction_table();
    //stack ops
    void push_to_stack(uint16_t num);
    void pop_from_stack(uint8_t& num_hi, uint8_t& num_lo);
    void pop_from_stack(uint16_t& num);

    //CPU states
    StateFunction current_state;
    void fetch_and_execute();

    //CPU-control opcodes
    Instruction JR(uint8_t offset, bool condition);
    Instruction JP(uint16_t destination, bool condition);
    Instruction CALL(uint16_t destination, bool condition);
    Instruction RET(bool condition);
    Instruction RST(uint8_t destination);
    Instruction PUSH(uint8_t num_hi, uint8_t num_lo);
    Instruction POP(uint8_t& num_hi, uint8_t& num_lo);
    Instruction DI();
    Instruction EI();
};

#endif
