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
    typedef std::function<Instruction(Register8&, const Register8&, FlagRegister&)> ALUOp;
    CPU& cpu;
    std::array<Instruction, 0x100> inst_table;
    void init_instruction_table();

    RegisterPair BC;
    RegisterPair DE;
    RegisterPair HL;
    RegisterPair AF;

    MemRegister BC_mem;
    MemRegister DE_mem;

    Immediate8 imm8; 
    Immediate16 imm16; 
    HighMemory h_mem; 

    std::vector<std::reference_wrapper<Register8>> regs;
    std::vector<ALUOp> alu_ops;

public: 
    Instruction decode(uint8_t opcode);
    Decoder(CPU&);
};

#endif