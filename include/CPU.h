#ifndef CPU_H
#define CPU_H

#include <cstdint>
#include <array>
#include <memory>
#include <string>
#include <iostream>
#include "Instruction.h"

class MMU;
class Instruction;
class Decoder;
class InterruptController;
enum class Interrupt : uint8_t;

enum class Flag {
    CARRY = 4, HALF_CARRY, NEGATIVE, ZERO
};

class CPU {
public: //methods
    CPU(MMU& mmu, InterruptController& ih);
    ~CPU();
    
    //cpu clocked in m-cycles (1 m-cycle = 4 t-states)
    void tick();
    
    void print_state();
    void log_state(std::ostream& os);

    uint8_t read_memory(uint16_t addr);
    void write_memory(uint16_t addr, uint8_t val);
    uint8_t fetch_byte();

    void set_flag(Flag fl, bool val) {
        F = (F & ~((uint8_t)1 << (int)fl)) | ((uint8_t)val << (int)fl);
    }
    bool get_flag(Flag fl) const {
        return (F >> (int)fl) & (uint8_t)1;
    }

    //skip to end of instruction
    void skip_inst() {skip_to_inst_end = true;}
    void prefix_mode() {cb_mode = true;}
    void halt();

public: //data
    //program counter and stack pointer
    uint16_t pc;    
    uint16_t sp;
    //data registers
    uint8_t A, B, C, D, E, H, L, F;
    bool IME = false;
    bool int_enable_pending = false;

    struct {
        uint8_t W, Z;
        uint16_t combined() const {return (W << 8) | Z;}
        void set(uint16_t val) {W = val >> 8; Z = val & 0xFF;}
    } latch;

private:
    using StateFunction = void (CPU::*)();  //pointer to state function

    int cycles;

    //CPU states
    StateFunction current_state;
    void initial_fetch();
    void execute_fetch();
    void interrupted();
    void halted();

    //execution
    Instruction* fetch_inst();
    void execute_inst();
    bool cb_mode = false;
    bool halt_bug = false;

    //facilities
    MMU& mmu;
    InterruptController& interrupt_controller;
    std::unique_ptr<Decoder> decoder;
    Instruction* current_inst;
    bool skip_to_inst_end = false;
};

#endif
