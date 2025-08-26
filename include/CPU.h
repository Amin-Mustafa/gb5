#ifndef CPU_H
#define CPU_H

#include <cstdint>
#include <array>
#include <memory>
#include <string>
#include <iostream>

class MMU;
class Instruction;
class Decoder;
class InterruptController;

enum class Flag {
    CARRY = 4, HALF_CARRY, NEGATIVE, ZERO
};

class CPU {
public: //methods
    CPU(MMU& mmu, InterruptController& ih);
    ~CPU();
    
    //cpu clocked in m-cycles (1 m-cycle = 4 t-states)
    void tick() {
        (this->*current_state)();
        cycles++;
    }
    
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

public: //data
    //program counter and stack pointer
    uint16_t pc;    
    uint16_t sp;
    //data registers
    uint8_t A, B, C, D, E, H, L, F;

    struct {
        uint8_t W, Z;
        uint16_t combined() const {return (W << 8) | Z;}
        void set(uint16_t val) {W = val >> 8; Z = val & 0xFF;}
    } latch;

private:
    using StateFunction = void (CPU::*)();  //pointer to state function

    int cycles;
    bool IME;

    //CPU states
    StateFunction current_state;
    void fetch_and_execute();
    void interrupted();

    //facilities
    MMU& mmu;
    std::unique_ptr<Decoder> decoder;
    InterruptController& interrupt_controller;
};

#endif
