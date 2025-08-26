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
    F{0xB0}, pc{0x100}, sp{0xFFFE},
    A{0x01}, B{0x00}, C{0x13}, D{0x00}, E{0xD8}, H{0x01}, L{0x4D}, 
    mmu{mmu}, current_state{&fetch_and_execute},
    decoder{std::make_unique<Decoder>(*this)}, interrupt_controller{interrupt_controller} 
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
    Instruction inst = decoder->decode(read_memory(pc));
    inst.execute();
    pc++;

    //check for interrupts before going to the next instruction
    if(IME && interrupt_controller.active()) {
        current_state = interrupted;
    }
}


void CPU::interrupted() {  
    for(int i = 0; i < InterruptController::num_interrupts; ++i) {
        auto req = static_cast<InterruptController::Interrupt>(i);
        if(interrupt_controller.requested(req)) {
            //disable IME and clear interrupt request
            IME = false; 
            interrupt_controller.clear(req);
            cycles += 1;

            push_to_stack(pc);
            jump(interrupt_controller.service_addr(req));

            current_state = fetch_and_execute;
            return;
        }
    }
}

void CPU::print_state(){
    std::cout << 
        std::format("A:{:02x}, B:{:02x}, C:{:02x}, D:{:02x}, E:{:02x}, H:{:02x}, L:{:02x}, ",
                    A.get(), B.get(), C.get(), D.get(), E.get(), H.get(), L.get()) <<
        std::format("[HL]:{:02x}, [DE]:{:02x}, ", mmu.read(pair(H, L)), mmu.read(pair(D,E))) <<
        std::format("PC:{:04x}, SP:{:04x}, F:{:d}{:d}{:d}{:d}\n", pc, sp.get(), F.get_flag(Flag::ZERO),
                    F.get_flag(Flag::NEGATIVE), F.get_flag(Flag::HALF_CARRY), F.get_flag(Flag::CARRY));
}

void CPU::log_state(std::ostream& stream) {
    stream << 
        std::format(
            "A: {:02X} F: {:02X} B: {:02X} C: {:02X} D: {:02X} E: {:02X} H: {:02X} L: {:02X} ",
            A.get(), F.get(), B.get(), C.get(), D.get(), E.get(), H.get(), L.get()
            ) 
        <<
        std::format(
            "SP: {:04X} PC: 00:{:04X} ({:02X} {:02X} {:02X} {:02X})\n", sp.get(), pc,
            mmu.read(pc), mmu.read(pc+1), mmu.read(pc+2), mmu.read(pc+3)
            );
}