#ifndef DISASSEMBLER_H
#define DISASSEMBLER_H

#include <cstdint>

class MMU;

class Disassembler {
private:
    MMU& mem;
    void disassemble_prefix_op(uint8_t opcode);
public:
    Disassembler(MMU& memory) :mem{memory} {}
    void disassemble_at(uint16_t pos);
};

#endif