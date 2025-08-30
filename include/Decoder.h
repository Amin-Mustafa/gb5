#ifndef DECODER_H
#define DECODER_H

#include <cstdint>
#include <memory>
#include <vector>
#include <functional>
#include "Instruction.h"

class CPU;

class Decoder {
private:    
    std::array<Instruction, 0x100> inst_table;
    std::array<Instruction, 0x100> cb_table;
    void init_instruction_table(CPU& cpu);
    void init_cb_table(CPU& cpu);
public: 
    Instruction* decode(uint8_t opcode);
    Decoder(CPU& cpu);
};

#endif