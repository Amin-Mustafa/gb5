#include <iostream>
#include <format>
#include <memory>
#include "../include/Arithmetic.h"
#include "../include/CPU.h"
#include "../include/MMU.h"
#include "../include/Instruction.h"
#include "../include/Disassembler.h"

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

CPU::CPU(MMU& memory):
    F{0xB0}, pc{0x100}, sp{0xFFFE},
    A{0x01}, B{0x00}, C{0x13}, D{0x00}, E{0xD8}, H{0x01}, L{0x4D}, 
    M{*this, H, L}, memory{memory}, current_state{&fetch_and_execute},
    decoder{new Decoder(*this)}, REG_IF(memory, 0xFF0F), REG_IE(memory, 0xFFFF) {}

uint8_t CPU::read_memory(uint16_t addr) {   
        cycles += 4;    //CPU memory access costs 4 cycles 
        return memory.read(addr);
    }
void CPU::write_memory(uint16_t addr, uint8_t val) { 
    cycles += 4;   
    memory.write(addr, val); 
}

void CPU::fetch_and_execute() {
    Instruction inst = decoder->decode(read_memory(pc));
    inst.execute();
    pc++;

    //check for interrupts
    if(IME) {
    //interrupts are enabled and...
        if(REG_IE.get() & REG_IF.get() & 0x1F) {  
            //an interrupt is requested
            current_state = interrupted;
        }
    }
}

void CPU::interrupted() {
    static Interrupt sources[5] = {
        Interrupt::VBLANK, Interrupt::LCD, Interrupt::SERIAL, Interrupt::TIMER, Interrupt::JOYPAD
    };
    for(int i = 0; i < 5; ++i) {
        if(interrupt_requested(sources[i])) {
            //CPU handles first interrupt it sees, then resumes normal operation
            //for one instruction, then does the same interrupt check again.
            service_interrupt(sources[i]); 
            current_state = fetch_and_execute;
            break;
        }
    }
}

void CPU::jump(uint16_t addr) {
    pc = addr;
    cycles += 4;
}

void CPU::pc_return() {
    pop_from_stack(pc);
    pc--;
    cycles += 4;    //cost of jump
}

void CPU::push_to_stack(uint16_t num) {
    uint16_t sp_val = sp.get();
    write_memory(sp_val - 1, num >> 8);
	write_memory(sp_val -2, num & 0xff);
	sp.set(sp_val - 2);
    cycles += 4; //pushing costs 4 cycles
    //total cycle cost = 12
}

void CPU::pop_from_stack(uint8_t& num_hi, uint8_t& num_lo) {
    uint16_t sp_val = sp.get();
    num_lo = read_memory(sp_val);
    num_hi = read_memory(sp_val + 1);
    sp.set(sp_val + 2); //popping costs nothing
    //total cycle cost = 8
}
void CPU::pop_from_stack(uint16_t& num) {
    uint16_t sp_val = sp.get();
    num = pair(read_memory(sp_val + 1), read_memory(sp_val));
    sp.set(sp_val + 2);
}

bool CPU::interrupt_requested(Interrupt kind) {
    return Arithmetic::bit_check(REG_IF.get(), static_cast<uint8_t>(kind));
}

void CPU::service_interrupt(Interrupt kind) {
    static uint16_t interrupt_vector[] = {0x40, 0x48, 0x50, 0x58, 0x60};
    uint8_t interrupt = static_cast<uint8_t>(kind);
    uint8_t requests = REG_IF.get();

    //clear request and IME flag
    requests = Arithmetic::bit_clear(requests, interrupt);
    REG_IF.set(requests);
    IME = false;
    
    //call to interrupt address
    push_to_stack(pc);
    jump(interrupt_vector[interrupt]);
}

//internal CPU operations
Instruction CPU::JR(const Register8& offset, FlagRegister::ConditionCheck cc) {
    return Instruction {
        [&offset, cc, this](){
            int8_t increment = static_cast<int8_t>(offset.get());
            if(cc(F)) {
                pc += increment;
                cycles += 4;    
            }
        }
    };
    //if condition, total cycles = initial fetch (4) + operand fetch (4) + jump (4) = 12
    //else, no jump; cycles = 8
}
Instruction CPU::JP(const Register16& destination, FlagRegister::ConditionCheck cc) {
    return Instruction {
        [&destination, cc, this](){
            uint16_t addr = destination.get();
            if(cc(F)) jump(addr - 1);
        }
    };
}
Instruction CPU::JPHL() {
    //special instruction; this particular jump takes no cycles
    return Instruction { [this](){ pc = pair(H,L) - 1; } };
}
Instruction CPU::CALL(const Register16& destination, FlagRegister::ConditionCheck cc) {
    return Instruction{
        [&destination, cc, this](){
            uint16_t addr = destination.get();
            if(cc(F)) {
                push_to_stack(pc+1);
                jump(addr - 1);
            }
        }
    };
}
Instruction CPU::PUSH(RegisterPair& rp) {
    return Instruction {
        [&rp, this](){
            push_to_stack(rp.get()); 
        }
    };
}
Instruction CPU::POP(RegisterPair& rp) {
    return Instruction {
        [&rp, this](){
            uint16_t num;
            pop_from_stack(num);
            rp.set(num);
        }
    };
}
Instruction CPU::RET() {
    return Instruction{[this](){pc_return();}};
}
Instruction CPU::RETI() {
    return Instruction{
        [this](){
            IME = true;
            pc_return();
        }
    };
}
Instruction CPU::RET_IF(FlagRegister::ConditionCheck cc) {
    return Instruction {
        [cc, this]() {
            cycles += 4;    //condition check cost
            if(cc(F)) pc_return();
        }
    };
}
Instruction CPU::RST(uint8_t destination) {
    return Instruction{
        [destination, this](){
            //an RST is a special kind of call that takes 16 cycles rather than 24
            //by having the destination built-in rather than fetched
            push_to_stack(pc+1);
            jump(destination - 1);
        }
    };
}
Instruction CPU::DI(){
    return Instruction{[this](){IME = false;}};
}
Instruction CPU::EI(){
    return Instruction{[this](){IME = true;}};
}

void CPU::print_state(){
    std::cout << 
        std::format("A:{:02x}, B:{:02x}, C:{:02x}, D:{:02x}, E:{:02x}, H:{:02x}, L:{:02x}, ",
                    A.get(), B.get(), C.get(), D.get(), E.get(), H.get(), L.get()) <<
        std::format("[HL]:{:02x}, [DE]:{:02x}, ", memory.read(pair(H, L)), memory.read(pair(D,E))) <<
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
            memory.read(pc), memory.read(pc+1), memory.read(pc+2), memory.read(pc+3)
            );
}