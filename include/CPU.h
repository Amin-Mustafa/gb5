#ifndef CPU_H
#define CPU_H

#include <cstdint>
#include <array>
#include <memory>
#include <string>
#include <iostream>
#include "Instruction.h"
#include "Memory/Bus.h"

class MMU;
class Instruction;
class InterruptController;
enum class Interrupt : uint8_t;

enum class Flag {
    CARRY = 4, HALF_CARRY, NEGATIVE, ZERO
};

class CPU {
public: //methods
    CPU(Bus& bus, MMU& mmu, InterruptController& ih);
    ~CPU();
    
    //cpu clocked in m-cycles (1 m-cycle = 4 t-states)
    void tick();

    void idle_m_cycle() { bus.cycle(); }

    uint8_t read_memory(uint16_t addr);
    void write_memory(uint16_t addr, uint8_t val);
    uint8_t fetch_byte();

    void set_flag(Flag fl, bool val) {
        F = (F & ~((uint8_t)1 << (int)fl)) | ((uint8_t)val << (int)fl);
    }
    bool get_flag(Flag fl) const {
        return (F >> (int)fl) & (uint8_t)1;
    }

    void prefix_mode() {cb_mode = true;}
    void halt();
    void schedule_ei() {ei_scheduled = true;}

public: //data
    //program counter and stack pointer
    uint16_t pc;    
    uint16_t sp;
    //data registers
    uint8_t A, B, C, D, E, H, L, F;
    bool IME = false;
private:
    using StateFunction = void (CPU::*)();  //pointer to state function

    int cycles;

    void service_interrupt(Interrupt irq);
    void execute(uint8_t opcode);
    void execute_cb(uint8_t opcode);
    bool halted;

    bool cb_mode = false;
    bool halt_bug = false;
    bool ei_scheduled = false;

    //facilities
    Bus& bus;
    InterruptController& interrupt_controller;
};

#endif
