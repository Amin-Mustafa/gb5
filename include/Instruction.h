#ifndef INSTRRUCTION_H
#define INSTRUCTION

#include <vector>
#include <string>
#include <functional>
#include <stdint.h>

namespace SM83 {

class Instruction {
public:
    typedef std::vector<std::reference_wrapper<uint8_t>> Args;
    typedef void(*Opfn)(Args&);

    Instruction()
        :_op{nullptr}, _args{}, _cycles{0}, _opname{""} {}
    Instruction(Opfn op, Args args, int cycles, std::string opname = "")
        :_op{op}, _args{args}, _cycles{cycles}, _opname{opname} {}
    
    void print();
    void execute();

private:
    Opfn _op;
    Args _args;
    int _cycles;
    std::string _opname;
};

//Standard operations, defined in ../src/Operations.cpp
void load8(Instruction::Args&);
}

#endif