#ifndef INSTRUCTION_H
#define INSTRUCTION

#include <string>
#include <iostream>
#include <functional>
#include <stdint.h>

namespace SM83 {

class CPU;

class Instruction {
public:
    typedef std::function<void()> Opfn;

    Instruction(Opfn operation, int length, int cycles, std::string opname = "")
        :op{std::move(operation)}, len{length}, cycles{0}, opname{opname} {}
    
    void print() {std::cout << opname << '\n';}
    void execute() { op(); }
    int length() {return len;}

private:
    Opfn op;
    int len;
    int cycles;
    std::string opname;
};

}

#endif