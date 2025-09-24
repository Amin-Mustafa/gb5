#include <iostream>
#include <format>
#include <memory>
#include <fstream>
#include "../include/Arithmetic.h"
#include "../include/Decoder.h"
#include "../include/CPU.h"
#include "../include/Memory/MMU.h"
#include "../include/Instruction.h"
#include "../include/Disassembler.h"
#include "../include/Memory/InterruptController.h"

using Arithmetic::pair;

CPU::CPU(MMU& mmu, InterruptController& interrupt_controller):
    pc{0xFF}, sp{0xFFFE},
    A{0x01}, B{0x00}, C{0x13}, D{0x00}, E{0xD8}, H{0x01}, L{0x4D}, F{0xB0},
    current_state{initial_fetch},
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

Instruction* CPU::fetch_inst() {
    static Disassembler dis(mmu);

    uint8_t opcode = fetch_byte();
    if(cb_mode) {
        cb_mode = false;
        dis.disassemble_prefix_op(opcode);
        print_state();
        return decoder->decode_cb(opcode);
    }
    dis.disassemble_at(pc);
    print_state();
    return decoder->decode(opcode);
}

void CPU::execute_inst() {
    if(skip_to_inst_end) {
        skip_to_inst_end = false;
        cycles = current_inst->length() - 1;
    }
    current_inst->execute_subop(*this, cycles);
}

void CPU::initial_fetch() {
    //fetch first instruction
    current_inst = fetch_inst();

    //begin execute fetch cycle
    current_state = execute_fetch;  
}

void CPU::execute_fetch() {
    execute_inst();
    cycles++;

    if(cycles >= current_inst->length()) {
        //once reach end, fetch next instruction in same cycle
        cycles = 0;
        current_inst = fetch_inst();
        if(int_enable_pending) {
            int_enable_pending = false;
            IME = true;
        }
        if(interrupt_pending()) {
            current_state = interrupted;
        }
    }
}

bool CPU::interrupt_pending() {
    return IME && interrupt_controller.active();
}

void CPU::interrupted() {
    //replace next instruction with the isr of pending interrupt
    Interrupt pending_int = interrupt_controller.pending();

    IME = false;
    interrupt_controller.clear(pending_int);

    switch(pending_int) {
        case Interrupt::VBLANK  : std::cout << "VBLANK INTERRUPT"   << '\n';    break;
        case Interrupt::LCD     : std::cout << "LCD INTERRUPT"      << '\n';    break;
        case Interrupt::SERIAL  : std::cout << "SERIAL INTERRUPT"   << '\n';    break;
        case Interrupt::TIMER   : std::cout << "TIMER INTERRUPT"    << '\n';    break;
        case Interrupt::JOYPAD  : std::cout << "JOYPAD INTERRUPT"   << '\n';    break;
        default: break;
    }

    current_inst = decoder->isr(pending_int);
    current_state = execute_fetch;
}

void CPU::print_state(){
    std::cout << 
        std::format(
            "A:{:02x}, B:{:02x}, C:{:02x}, D:{:02x}, E:{:02x}, H:{:02x}, L:{:02x}, ",
            A, B, C, D, E, H, L
        )
        <<
        std::format("[HL]:{:02x}, IME:{}, ", mmu.read(pair(H, L)), IME) 
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