#include <iostream>
#include <format>
#include <memory>
#include <fstream>
#include "Arithmetic.h"
#include "CPU.h"
#include "Memory/MMU.h"
#include "Memory/Bus.h"
#include "Instruction.h"
#include "Memory/InterruptController.h"
#include "Memory/Spaces.h"

using Arithmetic::pair;


CPU::CPU(Bus& bus, MMU& mmu, InterruptController& interrupt_controller):
    pc{0x100}, sp{0xFFFE},
    A{0x01}, B{0x00}, C{0x13}, D{0x00}, E{0xD8}, H{0x01}, L{0x4D}, F{0xB0},
    bus{bus},
    interrupt_controller{interrupt_controller}
    {
        bus.connect(*this);
    }

CPU::~CPU() = default;

uint8_t CPU::read_memory(uint16_t addr) {
    return bus.read( addr );
}
void CPU::write_memory(uint16_t addr, uint8_t val) {
    bus.write( addr, val );
}

uint8_t CPU::fetch_byte() {
    uint8_t byte = bus.read(pc);
    if(!halt_bug) {
        //the halt bug prevents the pc from increasing for one cycle
        pc++;   
    } else {
        halt_bug = false;
    }
    return byte;
}

void CPU::halt() {
    if(!IME && interrupt_controller.active()) {
        //HALT bug
        //do not enter HALT state at all
        halted = false;
        //pc not incremented after executing next instruction
        halt_bug = true;
    } else {
        halted = true;
    }
}

void CPU::tick() {
    if (interrupt_controller.active()) {
        //there is an interrupt pending
        halted = false; //wake up
        if (IME) {
            Interrupt pending = interrupt_controller.pending();
            interrupt_controller.clear(pending);
            service_interrupt(pending);
            return;
        } 
    }

    if(ei_scheduled) {
        ei_scheduled = false;
        IME = true;
    }

    if(halted) {
        idle_m_cycle();
        return;
    }

    if(bus.dma_active() && !addr_in_hram(pc)) {
        idle_m_cycle();
        return;
    }

    //fetch and execute
    uint8_t opcode = fetch_byte();
    if(cb_mode) {
        cb_mode = false;
        execute_cb(opcode);
    } else {
        execute(opcode);
    }
}

void CPU::service_interrupt(Interrupt irq) {
    IME = false;
    idle_m_cycle();
    idle_m_cycle();

    //push pc
    sp--;
    write_memory(sp, pc >> 8); 
    sp--;
    write_memory(sp, pc & 0xFF);

    uint16_t isr = 0x40 + (static_cast<uint8_t>(irq) * 8);
    pc = isr;

    idle_m_cycle();
}

namespace ConditionCode {
    bool NZ(const uint8_t& f) {return !Arithmetic::bit_check(f, (int)Flag::ZERO);}
    bool Z (const uint8_t& f) {return  Arithmetic::bit_check(f, (int)Flag::ZERO);}
    bool NC(const uint8_t& f) {return !Arithmetic::bit_check(f, (int)Flag::CARRY);}
    bool C (const uint8_t& f) {return  Arithmetic::bit_check(f, (int)Flag::CARRY);}
    bool ALWAYS(const uint8_t& f) {return true;}
}

void CPU::execute(uint8_t opcode) {
    using namespace Operation;
    switch(opcode) {
        case 0x00: NOP(*this);              break;  
        case 0x01: LD_rr_n16(*this, B, C);  break;
        case 0x02: LD_m_r(*this, B, C, A);  break;
        case 0x03: INC_rr(*this, B, C);     break;
        case 0x04: INC_r(*this, B);         break;
        case 0x05: DEC_r(*this, B);         break;
        case 0x06: LD_r_n(*this, B);        break;
        case 0x07: ROT_Inst_A(*this, Arithmetic::rot_left_circ);    break;
        case 0x08: LD_a16_SP(*this);        break;
        case 0x09: ADD_HL_rr(*this, B, C);  break;
        case 0x0A: LD_r_m(*this, A, B, C);  break;
        case 0x0B: DEC_rr(*this, B, C);     break;
        case 0x0C: INC_r(*this, C);         break;
        case 0x0D: DEC_r(*this, C);         break;
        case 0x0E: LD_r_n(*this, C);        break;
        case 0x0F: ROT_Inst_A(*this, Arithmetic::rot_right_circ);   break;

        case 0x10: NOP(*this);              break;   //TODO: STOP instruction
        case 0x11: LD_rr_n16(*this, D, E);  break;
        case 0x12: LD_m_r(*this, D, E, A);  break;
        case 0x13: INC_rr(*this, D, E);     break;
        case 0x14: INC_r(*this, D);         break;
        case 0x15: DEC_r(*this, D);         break;
        case 0x16: LD_r_n(*this, D);        break;
        case 0x17: ROT_Inst_A(*this, Arithmetic::rot_left); break;
        case 0x18: JR(*this, ConditionCode::ALWAYS);        break;
        case 0x19: ADD_HL_rr(*this, D, E);  break;
        case 0x1A: LD_r_m(*this, A, D, E);  break;
        case 0x1B: DEC_rr(*this, D, E);     break;
        case 0x1C: INC_r(*this, E);         break;
        case 0x1D: DEC_r(*this, E);         break;
        case 0x1E: LD_r_n(*this, E);        break;
        case 0x1F: ROT_Inst_A(*this, Arithmetic::rot_right);    break;

        case 0x20: JR(*this, ConditionCode::NZ);    break;
        case 0x21: LD_rr_n16(*this, H, L);  break;
        case 0x22: LD_HLinc_A(*this);       break;
        case 0x23: INC_rr(*this, H, L);     break;
        case 0x24: INC_r(*this, H);         break;
        case 0x25: DEC_r(*this, H);         break;
        case 0x26: LD_r_n(*this, H);        break;
        case 0x27: DAA(*this);              break;
        case 0x28: JR(*this, ConditionCode::Z); break;
        case 0x29: ADD_HL_rr(*this, H, L);  break;
        case 0x2A: LD_A_HLinc(*this);       break;
        case 0x2B: DEC_rr(*this, H, L);     break;
        case 0x2C: INC_r(*this, L);         break;
        case 0x2D: DEC_r(*this, L);         break;
        case 0x2E: LD_r_n(*this, L);        break;
        case 0x2F: CPL(*this);              break;

        case 0x30: JR(*this, ConditionCode::NC);    break;
        case 0x31: LD_SP_n16(*this);        break;
        case 0x32: LD_HLdec_A(*this);       break;
        case 0x33: INC_SP(*this);           break;
        case 0x34: INC_m(*this);            break;
        case 0x35: DEC_m(*this);            break;
        case 0x36: LD_m_n(*this, H, L);     break;
        case 0x37: SCF(*this);              break;
        case 0x38: JR(*this, ConditionCode::C); break;
        case 0x39: ADD_HL_SP(*this);        break;
        case 0x3A: LD_A_HLdec(*this);       break;
        case 0x3B: DEC_SP(*this);           break;
        case 0x3C: INC_r(*this, A);         break;
        case 0x3D: DEC_r(*this, A);         break;
        case 0x3E: LD_r_n(*this, A);        break;
        case 0x3F: CCF(*this);              break;

        case 0x40: LD_r_r(*this, B, B);     break;                
        case 0x41: LD_r_r(*this, B, C);     break;       
        case 0x42: LD_r_r(*this, B, D);     break;         
        case 0x43: LD_r_r(*this, B, E);     break;       
        case 0x44: LD_r_r(*this, B, H);     break;       
        case 0x45: LD_r_r(*this, B, L);     break;      
        case 0x46: LD_r_m(*this, B, H, L);  break;          
        case 0x47: LD_r_r(*this, B, A);     break;    
        case 0x48: LD_r_r(*this, C, B);     break;      
        case 0x49: LD_r_r(*this, C, C);     break;     
        case 0x4A: LD_r_r(*this, C, D);     break;         
        case 0x4B: LD_r_r(*this, C, E);     break;         
        case 0x4C: LD_r_r(*this, C, H);     break;          
        case 0x4D: LD_r_r(*this, C, L);     break;         
        case 0x4E: LD_r_m(*this, C, H, L);  break;          
        case 0x4F: LD_r_r(*this, C, A);     break;

        case 0x50: LD_r_r(*this, D, B);     break;                   
        case 0x51: LD_r_r(*this, D, C);     break;       
        case 0x52: LD_r_r(*this, D, D);     break;         
        case 0x53: LD_r_r(*this, D, E);     break;       
        case 0x54: LD_r_r(*this, D, H);     break;       
        case 0x55: LD_r_r(*this, D, L);     break;      
        case 0x56: LD_r_m(*this, D, H, L);  break;         
        case 0x57: LD_r_r(*this, D, A);     break;    
        case 0x58: LD_r_r(*this, E, B);     break;      
        case 0x59: LD_r_r(*this, E, C);     break;     
        case 0x5A: LD_r_r(*this, E, D);     break;         
        case 0x5B: LD_r_r(*this, E, E);     break;         
        case 0x5C: LD_r_r(*this, E, H);     break;          
        case 0x5D: LD_r_r(*this, E, L);     break;         
        case 0x5E: LD_r_m(*this, E, H, L);  break;             
        case 0x5F: LD_r_r(*this, E, A);     break;

        case 0x60: LD_r_r(*this, H, B);     break;                   
        case 0x61: LD_r_r(*this, H, C);     break;       
        case 0x62: LD_r_r(*this, H, D);     break;         
        case 0x63: LD_r_r(*this, H, E);     break;       
        case 0x64: LD_r_r(*this, H, H);     break;       
        case 0x65: LD_r_r(*this, H, L);     break;      
        case 0x66: LD_r_m(*this, H, H, L);  break;           
        case 0x67: LD_r_r(*this, H, A);     break;    
        case 0x68: LD_r_r(*this, L, B);     break;      
        case 0x69: LD_r_r(*this, L, C);     break;     
        case 0x6A: LD_r_r(*this, L, D);     break;         
        case 0x6B: LD_r_r(*this, L, E);     break;         
        case 0x6C: LD_r_r(*this, L, H);     break;          
        case 0x6D: LD_r_r(*this, L, L);     break;         
        case 0x6E: LD_r_m(*this, L, H, L);  break;             
        case 0x6F: LD_r_r(*this, L, A);     break;

        case 0x70: LD_m_r(*this, H, L, B);  break;                   
        case 0x71: LD_m_r(*this, H, L, C);  break;       
        case 0x72: LD_m_r(*this, H, L, D);  break;         
        case 0x73: LD_m_r(*this, H, L, E);  break;       
        case 0x74: LD_m_r(*this, H, L, H);  break;       
        case 0x75: LD_m_r(*this, H, L, L);  break;      
        case 0x76: HALT(*this);             break;    
        case 0x77: LD_m_r(*this, H, L, A);  break;    
        case 0x78: LD_r_r(*this, A, B);     break;     
        case 0x79: LD_r_r(*this, A, C);     break;    
        case 0x7A: LD_r_r(*this, A, D);     break;        
        case 0x7B: LD_r_r(*this, A, E);     break;        
        case 0x7C: LD_r_r(*this, A, H);     break;         
        case 0x7D: LD_r_r(*this, A, L);     break;        
        case 0x7E: LD_r_m(*this, A, H, L);  break;             
        case 0x7F: LD_r_r(*this, A, A);     break;

        case 0x80: ALU_Inst_r(*this, ALU::add_8, B);    break;                   
        case 0x81: ALU_Inst_r(*this, ALU::add_8, C);    break;       
        case 0x82: ALU_Inst_r(*this, ALU::add_8, D);    break;         
        case 0x83: ALU_Inst_r(*this, ALU::add_8, E);    break;       
        case 0x84: ALU_Inst_r(*this, ALU::add_8, H);    break;       
        case 0x85: ALU_Inst_r(*this, ALU::add_8, L);    break;      
        case 0x86: ALU_Inst_m(*this, ALU::add_8);       break;           
        case 0x87: ALU_Inst_r(*this, ALU::add_8, A);    break;    
        case 0x88: ALU_Inst_r(*this, ALU::adc_8, B);    break;      
        case 0x89: ALU_Inst_r(*this, ALU::adc_8, C);    break;     
        case 0x8A: ALU_Inst_r(*this, ALU::adc_8, D);    break;         
        case 0x8B: ALU_Inst_r(*this, ALU::adc_8, E);    break;         
        case 0x8C: ALU_Inst_r(*this, ALU::adc_8, H);    break;          
        case 0x8D: ALU_Inst_r(*this, ALU::adc_8, L);    break;         
        case 0x8E: ALU_Inst_m(*this, ALU::adc_8);       break;          
        case 0x8F: ALU_Inst_r(*this, ALU::adc_8, A);    break;
        
        case 0x90: ALU_Inst_r(*this, ALU::sub_8, B);    break;                   
        case 0x91: ALU_Inst_r(*this, ALU::sub_8, C);    break;       
        case 0x92: ALU_Inst_r(*this, ALU::sub_8, D);    break;         
        case 0x93: ALU_Inst_r(*this, ALU::sub_8, E);    break;       
        case 0x94: ALU_Inst_r(*this, ALU::sub_8, H);    break;       
        case 0x95: ALU_Inst_r(*this, ALU::sub_8, L);    break;      
        case 0x96: ALU_Inst_m(*this, ALU::sub_8);       break;        
        case 0x97: ALU_Inst_r(*this, ALU::sub_8, A);    break;    
        case 0x98: ALU_Inst_r(*this, ALU::sbc_8, B);    break;      
        case 0x99: ALU_Inst_r(*this, ALU::sbc_8, C);    break;     
        case 0x9A: ALU_Inst_r(*this, ALU::sbc_8, D);    break;         
        case 0x9B: ALU_Inst_r(*this, ALU::sbc_8, E);    break;         
        case 0x9C: ALU_Inst_r(*this, ALU::sbc_8, H);    break;          
        case 0x9D: ALU_Inst_r(*this, ALU::sbc_8, L);    break;         
        case 0x9E: ALU_Inst_m(*this, ALU::sbc_8);       break;           
        case 0x9F: ALU_Inst_r(*this, ALU::sbc_8, A);    break;

        case 0xA0: ALU_Inst_r(*this, ALU::and_8, B);    break;                   
        case 0xA1: ALU_Inst_r(*this, ALU::and_8, C);    break;       
        case 0xA2: ALU_Inst_r(*this, ALU::and_8, D);    break;         
        case 0xA3: ALU_Inst_r(*this, ALU::and_8, E);    break;       
        case 0xA4: ALU_Inst_r(*this, ALU::and_8, H);    break;       
        case 0xA5: ALU_Inst_r(*this, ALU::and_8, L);    break;      
        case 0xA6: ALU_Inst_m(*this, ALU::and_8);       break;          
        case 0xA7: ALU_Inst_r(*this, ALU::and_8, A);    break;    
        case 0xA8: ALU_Inst_r(*this, ALU::xor_8, B);    break;      
        case 0xA9: ALU_Inst_r(*this, ALU::xor_8, C);    break;     
        case 0xAA: ALU_Inst_r(*this, ALU::xor_8, D);    break;         
        case 0xAB: ALU_Inst_r(*this, ALU::xor_8, E);    break;         
        case 0xAC: ALU_Inst_r(*this, ALU::xor_8, H);    break;          
        case 0xAD: ALU_Inst_r(*this, ALU::xor_8, L);    break;         
        case 0xAE: ALU_Inst_m(*this, ALU::xor_8);       break;           
        case 0xAF: ALU_Inst_r(*this, ALU::xor_8, A);    break;

        case 0xB0: ALU_Inst_r(*this, ALU::or_8, B);     break;                   
        case 0xB1: ALU_Inst_r(*this, ALU::or_8, C);     break;       
        case 0xB2: ALU_Inst_r(*this, ALU::or_8, D);     break;         
        case 0xB3: ALU_Inst_r(*this, ALU::or_8, E);     break;       
        case 0xB4: ALU_Inst_r(*this, ALU::or_8, H);     break;       
        case 0xB5: ALU_Inst_r(*this, ALU::or_8, L);     break;      
        case 0xB6: ALU_Inst_m(*this, ALU::or_8);        break;         
        case 0xB7: ALU_Inst_r(*this, ALU::or_8, A);     break;   
        case 0xB8: ALU_Inst_r(*this, ALU::cp_8, B);     break;     
        case 0xB9: ALU_Inst_r(*this, ALU::cp_8, C);     break;    
        case 0xBA: ALU_Inst_r(*this, ALU::cp_8, D);     break;        
        case 0xBB: ALU_Inst_r(*this, ALU::cp_8, E);     break;        
        case 0xBC: ALU_Inst_r(*this, ALU::cp_8, H);     break;         
        case 0xBD: ALU_Inst_r(*this, ALU::cp_8, L);     break;        
        case 0xBE: ALU_Inst_m(*this, ALU::cp_8);        break;            
        case 0xBF: ALU_Inst_r(*this, ALU::cp_8, A);     break;

        case 0xC0: RET_IF(*this, ConditionCode::NZ);    break;
        case 0xC1: POP_rr(*this, B, C);                 break;
        case 0xC2: JP(*this, ConditionCode::NZ);        break;
        case 0xC3: JP(*this, ConditionCode::ALWAYS);    break;
        case 0xC4: CALL(*this, ConditionCode::NZ);      break;
        case 0xC5: PUSH_rr(*this, B, C);                break;
        case 0xC6: ALU_Inst_n(*this, ALU::add_8);       break;
        case 0xC7: RST(*this, 0x00);                    break;
        case 0xC8: RET_IF(*this, ConditionCode::Z);     break;
        case 0xC9: RET(*this);                          break;
        case 0xCA: JP(*this, ConditionCode::Z);         break;
        case 0xCB: PREFIX(*this);                       break;
        case 0xCC: CALL(*this, ConditionCode::Z);       break;
        case 0xCD: CALL(*this, ConditionCode::ALWAYS);  break;
        case 0xCE: ALU_Inst_n(*this, ALU::adc_8);       break;
        case 0xCF: RST(*this, 0x08);                    break;

        case 0xD0: RET_IF(*this, ConditionCode::NC);    break;
        case 0xD1: POP_rr(*this, D, E);                 break;  
        case 0xD2: JP(*this, ConditionCode::NC);        break;
        case 0xD3: NOP(*this);                          break;
        case 0xD4: CALL(*this, ConditionCode::NC);      break;
        case 0xD5: PUSH_rr(*this, D, E);                break;
        case 0xD6: ALU_Inst_n(*this, ALU::sub_8);       break;
        case 0xD7: RST(*this, 0x10);                    break;
        case 0xD8: RET_IF(*this, ConditionCode::C);     break;
        case 0xD9: RETI(*this);                         break;
        case 0xDA: JP(*this, ConditionCode::C);         break;
        case 0xDB: NOP(*this);                          break;
        case 0xDC: CALL(*this, ConditionCode::C);       break;
        case 0xDD: NOP(*this);                          break;
        case 0xDE: ALU_Inst_n(*this, ALU::sbc_8);       break;
        case 0xDF: RST(*this, 0x18);                    break;

        case 0xE0: LDH_n_A(*this);                      break;
        case 0xE1: POP_rr(*this, H, L);                 break;
        case 0xE2: LDH_C_A(*this);                      break;
        case 0xE3: NOP(*this);                          break;
        case 0xE4: NOP(*this);                          break;
        case 0xE5: PUSH_rr(*this, H, L);                break;
        case 0xE6: ALU_Inst_n(*this, ALU::and_8);       break;
        case 0xE7: RST(*this, 0x20);                    break;
        case 0xE8: ADD_SPe(*this);                      break;
        case 0xE9: JPHL(*this);                         break;
        case 0xEA: LD_a16_A(*this);                     break;
        case 0xEB: NOP(*this);                          break;
        case 0xEC: NOP(*this);                          break;
        case 0xED: NOP(*this);                          break;
        case 0xEE: ALU_Inst_n(*this, ALU::xor_8);       break;
        case 0xEF: RST(*this, 0x28);                    break;

        case 0xF0: LDH_A_n(*this);                      break;
        case 0xF1: POP_AF(*this);                       break;
        case 0xF2: LDH_A_C(*this);                      break;
        case 0xF3: DI(*this);                           break;
        case 0xF4: NOP(*this);                          break;
        case 0xF5: PUSH_rr(*this, A, F);                break;
        case 0xF6: ALU_Inst_n(*this, ALU::or_8);        break;
        case 0xF7: RST(*this, 0x30);                    break;
        case 0xF8: LD_HL_SPe(*this);                    break;
        case 0xF9: LD_SP_HL(*this);                     break;
        case 0xFA: LD_A_a16(*this);                     break;  
        case 0xFB: EI(*this);                           break;
        case 0xFC: NOP(*this);                          break;
        case 0xFD: NOP(*this);                          break;
        case 0xFE: ALU_Inst_n(*this, ALU::cp_8);        break;
        case 0xFF: RST(*this, 0x38);                    break;
    }
}

void CPU::execute_cb(uint8_t opcode) {
    using namespace Operation;
    using namespace Arithmetic;

    uint8_t* cpu_regs[] = {&B, &C, &D, &E, &H, &L, nullptr, &A};
    uint8_t* reg;
    switch(opcode) {
    case 0x00 ... 0x07:
        reg = cpu_regs[opcode % 8];  
        if(reg) ROT_Inst_r(*this, rot_left_circ, *reg);
        else    ROT_Inst_m(*this, rot_left_circ);
        break;

    case 0x08 ... 0x0F: 
        reg = cpu_regs[opcode % 8];
        if(reg) ROT_Inst_r(*this, rot_right_circ, *reg);
        else    ROT_Inst_m(*this, rot_right_circ);
        break;
    
    case 0x10 ... 0x17:
        reg = cpu_regs[opcode % 8];
        if(reg) ROT_Inst_r(*this, rot_left, *reg);
        else    ROT_Inst_m(*this, rot_left);
        break;

    case 0x18 ... 0x1F:
        reg = cpu_regs[opcode % 8];
        if(reg) ROT_Inst_r(*this, rot_right, *reg);
        else    ROT_Inst_m(*this, rot_right);
        break;
    
    case 0x20 ... 0x27:
        reg = cpu_regs[opcode % 8];
        if(reg) ROT_Inst_r(*this, shift_left_arithmetic, *reg);
        else    ROT_Inst_m(*this, shift_left_arithmetic);
        break;
    
    case 0x28 ... 0x2F:
        reg = cpu_regs[opcode % 8];
        if(reg) ROT_Inst_r(*this, shift_right_arithmetic, *reg);
        else    ROT_Inst_m(*this, shift_right_arithmetic);
        break;

    case 0x30 ... 0x37:
        reg = cpu_regs[opcode % 8];
        if(reg) SWAP_r(*this, *reg);
        else    SWAP_m(*this);
        break;
    
    case 0x38 ... 0x3F:
        reg = cpu_regs[opcode % 8];
        if(reg) ROT_Inst_r(*this, shift_right_logical, *reg);
        else    ROT_Inst_m(*this, shift_right_logical);
        break;
    
    case 0x40 ... 0x7F:
        reg = cpu_regs[opcode % 8];
        if(reg) BIT_r(*this, *reg, (opcode - 0x40) / 8);
        else    BIT_m(*this, (opcode - 0x40) / 8);
        break;

    case 0x80 ... 0xBF:
        reg = cpu_regs[opcode % 8];
        if(reg) RES_r(*this, *reg, (opcode - 0x80) / 8);
        else    RES_m(*this, (opcode - 0x80) / 8);
        break;

    case 0xC0 ... 0xFF:
        reg = cpu_regs[opcode % 8];
        if(reg) SET_r(*this, *reg, (opcode - 0xC0) / 8);
        else    SET_m(*this, (opcode - 0xC0) / 8);
        break;
    }
}