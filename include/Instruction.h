#ifndef INSTRUCTION_H
#define INSTRUCTION

#include <vector>
#include <string>
#include <functional>
#include <variant>
#include <stdint.h>
#include "CPU.h"

namespace SM83 {

class CPU;

class Instruction {
public:
    typedef std::variant<std::reference_wrapper<uint8_t>, std::reference_wrapper<uint16_t>> Operand;
    typedef std::vector<Operand> Args;
    typedef void(*Argfn)(Args&);    //for pure argument operations
    typedef void(CPU::*CPUfn)();    //for CPU-dependent operations

    Instruction(CPUfn cpu_op, int length, int cycles, std::string opname = "")
        :op{cpu_op}, args{}, len{length}, cycles{0}, opname{""} {}
    Instruction(Argfn arg_op, Args args, int length, int cycles, std::string opname = "")
        :op{arg_op}, args{args}, len{length}, cycles{cycles}, opname{opname} {}
    
    void print();
    void execute(CPU&);
    int length() {return len;}

private:
    std::variant<Argfn, CPUfn> op;
    Args args;
    int len;
    int cycles;
    std::string opname;
};

//Standard operations, defined in ../src/Operations.cpp
void nop(Instruction::Args&);
void load8(Instruction::Args&);
void load16(Instruction::Args&);
void load_pair(Instruction::Args&);
void inc8(Instruction::Args&); 
void inc16(Instruction::Args&);
 
}

#endif