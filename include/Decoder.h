#ifndef DECODER_H
#define DECODER_H

#include <cstdint>
#include <memory>
#include <vector>
#include <functional>

class CPU;
class Register8;
class Register16;
class Instruction;

class Decoder {
private:
    CPU& cpu;
    std::vector<std::reference_wrapper<Register8>> regs;
    std::vector<std::unique_ptr<Register16>> reg_pairs;
public: 
    Instruction decode(uint8_t opcode);
    Decoder(CPU&);
};

#endif