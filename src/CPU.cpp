#include <iostream>
#include <format>
#include <memory>
#include "../include/Arithmetic.h"
#include "../include/Decoder.h"
#include "../include/CPU.h"
#include "../include/Memory/MMU.h"
#include "../include/Instruction.h"
#include "../include/Disassembler.h"
#include "../include/Memory/InterruptController.h"

constexpr uint16_t pair(uint8_t hi, uint8_t lo) {
    return (hi << 8) | lo;
}
constexpr void inc_pair(uint8_t& hi, uint8_t& lo){
    uint16_t pair = (hi << 8) | lo;
    pair++;
    hi = pair >> 8;
    lo = pair & 0xff;
}
constexpr void dec_pair(uint8_t& hi, uint8_t& lo){
    uint16_t pair = (hi << 8) | lo;
    pair--;
    hi = pair >> 8;
    lo = pair & 0xff;
}

CPU::CPU(MMU& mmu, InterruptController& interrupt_controller):
    pc{0xFF}, sp{0xFFFE},
    A{0x01}, B{0x00}, C{0x13}, D{0x00}, E{0xD8}, H{0x01}, L{0x4D}, F{0xB0},
    current_state{fetch_and_execute},
    mmu{mmu},
    interrupt_controller{interrupt_controller},
    decoder{std::make_unique<Decoder>(*this)},
    current_inst{nullptr}
    {}

CPU::~CPU() = default;

uint8_t CPU::read_memory(uint16_t addr) {   
        return mmu.read(addr);
    }
void CPU::write_memory(uint16_t addr, uint8_t val) { 
    mmu.write(addr, val); 
}
uint8_t CPU::fetch_byte() {
    pc++;
    uint8_t byte = read_memory(pc);
    return byte;
}

void CPU::fetch_and_execute() {
    static Disassembler dis(mmu);
    if(cycles == 0) {
        //fetch instruction and execute first subop in first tick
        uint8_t opcode = fetch_byte();
        current_inst = decoder->decode(opcode);
        dis.disassemble_at(pc);
        print_state();
    }

    if(skip_to_inst_end) {
        cycles = current_inst->length() - 1;
    }

    //execute 1 subop
    current_inst->execute_subop(*this, cycles);
    cycles++;

    if(cycles >= current_inst->length()) {
        //once reach end, fetch next instruction
        cycles = 0;
    }
}

void CPU::print_state(){
    std::cout << 
        std::format(
            "A:{:02x}, B:{:02x}, C:{:02x}, D:{:02x}, E:{:02x}, H:{:02x}, L:{:02x}, ",
            A, B, C, D, E, H, L
        )
        <<
        std::format("[HL]:{:02x}, [DE]:{:02x}, ", mmu.read(pair(H, L)), mmu.read(pair(D,E))) 
        <<
        std::format(
            "PC:{:04x}, SP:{:04x}, F:{:d}{:d}{:d}{:d}\n", pc, sp, get_flag(Flag::ZERO),
            get_flag(Flag::NEGATIVE), get_flag(Flag::HALF_CARRY), get_flag(Flag::CARRY)
        );
}

void CPU::log_state(std::ostream& stream) {
    stream << 
        std::format(
            "A: {:02X} F: {:02X} B: {:02X} C: {:02X} D: {:02X} E: {:02X} H: {:02X} L: {:02X} ",
            A, F, B, C, D, E, H, L
        ) 
        <<
        std::format(
            "SP: {:04X} PC: 00:{:04X} ({:02X} {:02X} {:02X} {:02X})\n", sp, pc,
            read_memory(pc), read_memory(pc+1), read_memory(pc+2), read_memory(pc+3)
        );
}