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

class Operand {
public:
    using OperandVariant = std::variant<
        std::reference_wrapper<uint8_t>, std::reference_wrapper<uint16_t>, uint8_t, uint16_t
    >;

    // Constructor for register references
    Operand(std::reference_wrapper<uint8_t> reg) : operand(reg) {}
    Operand(std::reference_wrapper<uint16_t> reg16) : operand(reg16) {}

    // Constructor for immediate values
    Operand(uint8_t imm) : operand(imm) {}
    Operand(uint16_t imm) : operand(imm) {}

    template <typename T>
    T& get() {
        return std::get<T>(operand);  // Extract the value of the given type
    }

    template <typename T>
    const T& get() const {
        return std::get<T>(operand); 
    }

private:
    OperandVariant operand;
};

class Instruction {
public:
    typedef std::vector<Operand> Args;
    typedef std::function<void(Args&)> Opfn;

    Instruction(Opfn operation, Args args, int length, int cycles, std::string opname = "")
        :op{std::move(operation)}, args{std::move(args)}, len{length}, cycles{0}, opname{opname} {}
    
    void print() {std::cout << opname << '\n';}
    void execute() {op(args);}
    int length() {return len;}

private:
    Opfn op;
    Args args;
    int len;
    int cycles;
    std::string opname;
};

//Standard operations, defined in ../src/Operations.cpp
void nop(Instruction::Args&);
void load8(Instruction::Args&);
}

#endif