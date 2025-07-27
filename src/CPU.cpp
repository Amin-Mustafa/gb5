#include <iostream>
#include <format>
#include <memory>
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
    F{0x80}, pc{0xFA}, sp{0xFFFE},
    A{0x01}, B{0x00}, C{0x13}, D{0x00}, E{0xD8}, H{0x01}, L{0x4D}, 
    M{*this, H, L}, memory{memory}, current_state{&fetch_and_execute},
    decoder{new Decoder(*this)} {}

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
}

void CPU::jump(uint16_t addr) {
    pc = addr;
    cycles += 4;
}

void CPU::pc_return() {
    pop_from_stack(pc);
    cycles += 4;    //cost of jump
}

void CPU::push_to_stack(uint16_t num) {
    write_memory(sp-1, num >> 8);
	write_memory(sp-2, num & 0xff);
	sp.set(sp-2);
    cycles += 4; //pushing costs 4 cycles
    //total cycle cost = 12
}

void CPU::pop_from_stack(uint8_t& num_hi, uint8_t& num_lo) {
    num_lo = read_memory(sp);
    num_hi = read_memory(sp+1);
    sp.set(sp+2); //popping costs nothing
    //total cycle cost = 8
}
void CPU::pop_from_stack(uint16_t& num) {
    num = pair(read_memory(sp+1), read_memory(sp));
    sp.set(sp+2);
}

//internal CPU operations
Instruction CPU::JR(const Register8& offset, bool condition) {
    if(condition) {
        return Instruction{
            [&](){ 
                pc += (int8_t)offset.get();
                cycles += 4; //jump base cost = 4
            }
            //total cost = inst fetch (4) + operand fetch (4) + jump base cost (4)
            // =  12 cycles
        };
    }
    else return Instruction(); 
}
Instruction CPU::JP(const Register16& destination, bool condition) {
    if(condition) {
        uint16_t addr = destination.get();
        return Instruction{ [&](){ jump(addr - 1);} };
    }
    return Instruction();
}
Instruction CPU::JPHL() {
    //special instruction; this particular jump takes no cycles
    return Instruction { [&](){ pc = pair(H,L); } };
}
Instruction CPU::CALL(const Register16& destination, bool condition) {
    if(condition) {
        return Instruction{
            [&]() {
                uint16_t addr = destination.get();
                push_to_stack(pc+2);
                jump(addr - 1);
            }
        };
    }
    else return Instruction();
}
Instruction CPU::PUSH(RegisterPair& rp) {
    return Instruction {
        [&](){
            push_to_stack(rp.get()); 
        }
    };
}
Instruction CPU::POP(RegisterPair& rp) {
    return Instruction {
        [&](){
            uint16_t num = rp.get();
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
            int_enable = true;
            pc_return();
        }
    };
}
Instruction CPU::RET_IF(bool condition) {
    //conditional return has an extra 4 cycle cost for hardware reasons
    if(condition) {
        return Instruction{
            [&](){
                pc_return();
            },
            4
    };
    }
    else return Instruction{[&](){cycles += 4;}};
}
Instruction CPU::RST(uint8_t destination) {
    return Instruction{
        [&](){
            //an RST is a special kind of jump that takes 16 cycles rather than 4
            //its only advantage over JP is only needing one byte rather than 3
            //so it can be used in interrupts
            pc = destination;
        },
        16
    };
}
Instruction CPU::DI(){
    return Instruction{[this](){int_enable = false;}};
}
Instruction CPU::EI(){
    return Instruction{[this](){int_enable = true;}};
}

void CPU::print_state(){
    std::cout << 
        std::format("A:{:02x}, B:{:02x}, C:{:02x}, D:{:02x}, E:{:02x}, H:{:02x}, L:{:02x}, ",
                    A.get(), B.get(), C.get(), D.get(), E.get(), H.get(), L.get()) <<
        std::format("[HL]:{:02x}, ", memory.read(pair(H, L))) <<
        std::format("PC:{:04x}, SP:{:04x}, F:{:d}{:d}{:d}{:d}\n", pc, sp.get(), F.get_flag(Flag::ZERO),
                    F.get_flag(Flag::NEGATIVE), F.get_flag(Flag::HALF_CARRY), F.get_flag(Flag::CARRY));
}