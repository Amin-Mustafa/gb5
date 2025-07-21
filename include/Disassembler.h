#ifndef DISASSEMBLER_H
#define DISASSEMBLER_H

#include <cstdint>

class MMU;

class Disassembler {
private:
    MMU& mem;

public:
    Disassembler(MMU& memory) :mem{memory} {}
    void disassemble_at(uint16_t pos);
};

#endif