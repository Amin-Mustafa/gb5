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

    std::array<uint8_t*, 8> regs;
public: 
    Instruction* decode(uint8_t opcode) {return &inst_table[opcode];}
    Instruction* decode_cb(uint8_t opcode) {return &cb_table[opcode];}
    Decoder(CPU& cpu);
};

#endif