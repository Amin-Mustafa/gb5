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
    typedef std::vector<std::reference_wrapper<uint8_t>> Args;
    typedef void(*Argfn)(Args&);    //for pure argument operations
    typedef void(CPU::*CPUfn)();    //for CPU-dependent operations

    Instruction(CPUfn cpu_op, int cycles, std::string opname = "")
        :op{cpu_op}, args{}, cycles{0}, opname{""} {}
    Instruction(Argfn arg_op, Args args, int cycles, std::string opname = "")
        :op{arg_op}, args{args}, cycles{cycles}, opname{opname} {}
    
    void print();
    void execute(CPU&);

private:
    std::variant<Argfn, CPUfn> op;
    Args args;
    int cycles;
    std::string opname;
};

//Standard operations, defined in ../src/Operations.cpp
void load8(Instruction::Args&);
void load16(Instruction::Args&);
}

#endif