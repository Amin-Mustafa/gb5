#ifndef DECODER_H
#define DECODER_H

#include <cstdint>
#include <memory>
#include <vector>
#include <functional>
#include "Register.h"
#include "Instruction.h"

class CPU;

class Decoder {
private:
    typedef Instruction (*ALUOp)(Register8&, const Register8&, FlagRegister&);
    typedef Instruction (*ShiftOp)(Register8&, FlagRegister&);
    CPU& cpu;
    std::array<Instruction, 0x100> inst_table;
    std::array<Instruction, 0x100> cb_table;
    void init_instruction_table();
    void init_cb_table();

    std::vector<std::reference_wrapper<Register8>> regs;
    std::vector<ALUOp> alu_ops;
    std::vector<ShiftOp> shift_ops;

    RegisterPair BC, DE, HL, AF;

    MemRegister BC_mem, DE_mem;

    Immediate8 imm8; 
    Immediate16 imm16; 
    HighMemory h_mem; 

public: 
    Instruction decode(uint8_t opcode);
    Decoder(CPU&);
};

#endif