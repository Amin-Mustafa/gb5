#ifndef DECODER_H
#define DECODER_H

#include <cstdint>
#include <memory>
#include <vector>
#include <functional>
#include "Instruction.h"

class CPU;
enum class Interrupt : uint8_t;

class Decoder {
private:    
    std::array<Instruction, 0x100> inst_table;
    std::array<Instruction, 0x100> cb_table;
    std::array<Instruction, 5> ivt;
    void init_instruction_table(CPU& cpu);
    void init_cb_table(CPU& cpu);
    void init_ivt(CPU& cpu);

    std::array<uint8_t*, 8> regs;
public: 
    Instruction* decode(uint8_t opcode) {
        return &inst_table[opcode];
    }
    Instruction* decode_cb(uint8_t opcode) {
        return &cb_table[opcode];
    }
    Instruction* isr(Interrupt interrupt) {
        return &ivt[static_cast<uint8_t>(interrupt)];
    }
    Decoder(CPU& cpu);
};

#endif