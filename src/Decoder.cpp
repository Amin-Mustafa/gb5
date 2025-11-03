#include "../include/Decoder.h"
#include "../include/CPU.h"
#include "../include/Instruction.h"
#include "../include/Arithmetic.h"
#include "../include/Memory/InterruptController.h"


using std::ref;
using namespace Operation;

constexpr uint8_t VBLANK_ISR = 0x40;
constexpr uint8_t LCD_ISR    = 0x48;
constexpr uint8_t TIMER_ISR  = 0x50;
constexpr uint8_t SERIAL_ISR = 0x58;
constexpr uint8_t JOYPAD_ISR = 0x60;

bool NZ(const uint8_t& f) {return !Arithmetic::bit_check(f, (int)Flag::ZERO);}
bool Z (const uint8_t& f) {return  Arithmetic::bit_check(f, (int)Flag::ZERO);}
bool NC(const uint8_t& f) {return !Arithmetic::bit_check(f, (int)Flag::CARRY);}
bool C (const uint8_t& f) {return  Arithmetic::bit_check(f, (int)Flag::CARRY);}
bool no_cond(const uint8_t& f) {return true;}


Decoder::Decoder(CPU& cpu)
    :regs{&cpu.B, &cpu.C, &cpu.D, &cpu.E, &cpu.H, &cpu.L, nullptr, &cpu.A}
    {
        init_instruction_table(cpu);
        init_cb_table(cpu);
        init_ivt(cpu);
    }
    
void Decoder::init_instruction_table(CPU& cpu) {
    using namespace Operation;

    inst_table[0x00] = NOP();
    inst_table[0x01] = LD_rr_n16(cpu.B, cpu.C);
    inst_table[0x02] = LD_m_r(cpu.B, cpu.C, cpu.A);
    inst_table[0x03] = INC_rr(cpu.B, cpu.C);
    inst_table[0x04] = INC_r(cpu.B);
    inst_table[0x05] = DEC_r(cpu.B);
    inst_table[0x06] = LD_r_n(cpu.B);
    inst_table[0x07] = ROT_Inst_A(Arithmetic::rot_left_circ);
    inst_table[0x08] = LD_a16_SP();
    inst_table[0x09] = ADD_HL_rr(cpu.B, cpu.C);
    inst_table[0x0A] = LD_r_m(cpu.A, cpu.B, cpu.C);
    inst_table[0x0B] = DEC_rr(cpu.B, cpu.C);
    inst_table[0x0C] = INC_r(cpu.C);
    inst_table[0x0D] = DEC_r(cpu.C);
    inst_table[0x0E] = LD_r_n(cpu.C);
    inst_table[0x0F] = ROT_Inst_A(Arithmetic::rot_right_circ);

    inst_table[0x10] = NOP();   //TODO: STOP instruction
    inst_table[0x11] = LD_rr_n16(cpu.D, cpu.E);
    inst_table[0x12] = LD_m_r(cpu.D, cpu.E, cpu.A);
    inst_table[0x13] = INC_rr(cpu.D, cpu.E);
    inst_table[0x14] = INC_r(cpu.D);
    inst_table[0x15] = DEC_r(cpu.D);
    inst_table[0x16] = LD_r_n(cpu.D);
    inst_table[0x17] = ROT_Inst_A(Arithmetic::rot_left);
    inst_table[0x18] = JR(no_cond);
    inst_table[0x19] = ADD_HL_rr(cpu.D, cpu.E);
    inst_table[0x1A] = LD_r_m(cpu.A, cpu.D, cpu.E);
    inst_table[0x1B] = DEC_rr(cpu.D, cpu.E);
    inst_table[0x1C] = INC_r(cpu.E);
    inst_table[0x1D] = DEC_r(cpu.E);
    inst_table[0x1E] = LD_r_n(cpu.E);
    inst_table[0x1F] = ROT_Inst_A(Arithmetic::rot_right);

    inst_table[0x20] = JR(NZ);
    inst_table[0x21] = LD_rr_n16(cpu.H, cpu.L);
    inst_table[0x22] = LD_HLinc_A();
    inst_table[0x23] = INC_rr(cpu.H, cpu.L);
    inst_table[0x24] = INC_r(cpu.H);
    inst_table[0x25] = DEC_r(cpu.H);
    inst_table[0x26] = LD_r_n(cpu.H);
    inst_table[0x27] = DAA();
    inst_table[0x28] = JR(Z);
    inst_table[0x29] = ADD_HL_rr(cpu.H, cpu.L);
    inst_table[0x2A] = LD_A_HLinc();
    inst_table[0x2B] = DEC_rr(cpu.H, cpu.L);
    inst_table[0x2C] = INC_r(cpu.L);
    inst_table[0x2D] = DEC_r(cpu.L);
    inst_table[0x2E] = LD_r_n(cpu.L);
    inst_table[0x2F] = CPL();

    inst_table[0x30] = JR(NC);   
    inst_table[0x31] = LD_SP_n16();
    inst_table[0x32] = LD_HLdec_A();
    inst_table[0x33] = INC_SP();
    inst_table[0x34] = INC_m();
    inst_table[0x35] = DEC_m();
    inst_table[0x36] = LD_m_n(cpu.H, cpu.L);
    inst_table[0x37] = SCF();
    inst_table[0x38] = JR(C);
    inst_table[0x39] = ADD_HL_SP();
    inst_table[0x3A] = LD_A_HLdec();
    inst_table[0x3B] = DEC_SP();
    inst_table[0x3C] = INC_r(cpu.A);
    inst_table[0x3D] = DEC_r(cpu.A);
    inst_table[0x3E] = LD_r_n(cpu.A);
    inst_table[0x3F] = CCF();

    inst_table[0x40] = LD_r_r(cpu.B, cpu.B);                   
    inst_table[0x41] = LD_r_r(cpu.B, cpu.C);       
    inst_table[0x42] = LD_r_r(cpu.B, cpu.D);         
    inst_table[0x43] = LD_r_r(cpu.B, cpu.E);       
    inst_table[0x44] = LD_r_r(cpu.B, cpu.H);       
    inst_table[0x45] = LD_r_r(cpu.B, cpu.L);      
    inst_table[0x46] = LD_r_m(cpu.B, cpu.H, cpu.L);           
    inst_table[0x47] = LD_r_r(cpu.B, cpu.A);    
    inst_table[0x48] = LD_r_r(cpu.C, cpu.B);      
    inst_table[0x49] = LD_r_r(cpu.C, cpu.C);     
    inst_table[0x4A] = LD_r_r(cpu.C, cpu.D);         
    inst_table[0x4B] = LD_r_r(cpu.C, cpu.E);         
    inst_table[0x4C] = LD_r_r(cpu.C, cpu.H);          
    inst_table[0x4D] = LD_r_r(cpu.C, cpu.L);         
    inst_table[0x4E] = LD_r_m(cpu.C, cpu.H, cpu.L);             
    inst_table[0x4F] = LD_r_r(cpu.C, cpu.A);     
    
    inst_table[0x50] = LD_r_r(cpu.D, cpu.B);                   
    inst_table[0x51] = LD_r_r(cpu.D, cpu.C);       
    inst_table[0x52] = LD_r_r(cpu.D, cpu.D);         
    inst_table[0x53] = LD_r_r(cpu.D, cpu.E);       
    inst_table[0x54] = LD_r_r(cpu.D, cpu.H);       
    inst_table[0x55] = LD_r_r(cpu.D, cpu.L);      
    inst_table[0x56] = LD_r_m(cpu.D, cpu.H, cpu.L);           
    inst_table[0x57] = LD_r_r(cpu.D, cpu.A);    
    inst_table[0x58] = LD_r_r(cpu.E, cpu.B);      
    inst_table[0x59] = LD_r_r(cpu.E, cpu.C);     
    inst_table[0x5A] = LD_r_r(cpu.E, cpu.D);         
    inst_table[0x5B] = LD_r_r(cpu.E, cpu.E);         
    inst_table[0x5C] = LD_r_r(cpu.E, cpu.H);          
    inst_table[0x5D] = LD_r_r(cpu.E, cpu.L);         
    inst_table[0x5E] = LD_r_m(cpu.E, cpu.H, cpu.L);             
    inst_table[0x5F] = LD_r_r(cpu.E, cpu.A);

    inst_table[0x60] = LD_r_r(cpu.H, cpu.B);                   
    inst_table[0x61] = LD_r_r(cpu.H, cpu.C);       
    inst_table[0x62] = LD_r_r(cpu.H, cpu.D);         
    inst_table[0x63] = LD_r_r(cpu.H, cpu.E);       
    inst_table[0x64] = LD_r_r(cpu.H, cpu.H);       
    inst_table[0x65] = LD_r_r(cpu.H, cpu.L);      
    inst_table[0x66] = LD_r_m(cpu.H, cpu.H, cpu.L);           
    inst_table[0x67] = LD_r_r(cpu.H, cpu.A);    
    inst_table[0x68] = LD_r_r(cpu.L, cpu.B);      
    inst_table[0x69] = LD_r_r(cpu.L, cpu.C);     
    inst_table[0x6A] = LD_r_r(cpu.L, cpu.D);         
    inst_table[0x6B] = LD_r_r(cpu.L, cpu.E);         
    inst_table[0x6C] = LD_r_r(cpu.L, cpu.H);          
    inst_table[0x6D] = LD_r_r(cpu.L, cpu.L);         
    inst_table[0x6E] = LD_r_m(cpu.L, cpu.H, cpu.L);             
    inst_table[0x6F] = LD_r_r(cpu.L, cpu.A);
    
    inst_table[0x70] = LD_m_r(cpu.H, cpu.L, cpu.B);                   
    inst_table[0x71] = LD_m_r(cpu.H, cpu.L, cpu.C);       
    inst_table[0x72] = LD_m_r(cpu.H, cpu.L, cpu.D);         
    inst_table[0x73] = LD_m_r(cpu.H, cpu.L, cpu.E);       
    inst_table[0x74] = LD_m_r(cpu.H, cpu.L, cpu.H);       
    inst_table[0x75] = LD_m_r(cpu.H, cpu.L, cpu.L);      
    inst_table[0x76] = HALT();        
    inst_table[0x77] = LD_m_r(cpu.H, cpu.L, cpu.A);    
    inst_table[0x78] = LD_r_r(cpu.A, cpu.B);      
    inst_table[0x79] = LD_r_r(cpu.A, cpu.C);     
    inst_table[0x7A] = LD_r_r(cpu.A, cpu.D);         
    inst_table[0x7B] = LD_r_r(cpu.A, cpu.E);         
    inst_table[0x7C] = LD_r_r(cpu.A, cpu.H);          
    inst_table[0x7D] = LD_r_r(cpu.A, cpu.L);         
    inst_table[0x7E] = LD_r_m(cpu.A, cpu.H, cpu.L);             
    inst_table[0x7F] = LD_r_r(cpu.A, cpu.A);

    inst_table[0x80] = ALU_Inst_r(ALU::add_8, cpu.B);                   
    inst_table[0x81] = ALU_Inst_r(ALU::add_8, cpu.C);       
    inst_table[0x82] = ALU_Inst_r(ALU::add_8, cpu.D);         
    inst_table[0x83] = ALU_Inst_r(ALU::add_8, cpu.E);       
    inst_table[0x84] = ALU_Inst_r(ALU::add_8, cpu.H);       
    inst_table[0x85] = ALU_Inst_r(ALU::add_8, cpu.L);      
    inst_table[0x86] = ALU_Inst_m(ALU::add_8);           
    inst_table[0x87] = ALU_Inst_r(ALU::add_8, cpu.A);    
    inst_table[0x88] = ALU_Inst_r(ALU::adc_8, cpu.B);      
    inst_table[0x89] = ALU_Inst_r(ALU::adc_8, cpu.C);     
    inst_table[0x8A] = ALU_Inst_r(ALU::adc_8, cpu.D);         
    inst_table[0x8B] = ALU_Inst_r(ALU::adc_8, cpu.E);         
    inst_table[0x8C] = ALU_Inst_r(ALU::adc_8, cpu.H);          
    inst_table[0x8D] = ALU_Inst_r(ALU::adc_8, cpu.L);         
    inst_table[0x8E] = ALU_Inst_m(ALU::adc_8);             
    inst_table[0x8F] = ALU_Inst_r(ALU::adc_8, cpu.A);

    inst_table[0x90] = ALU_Inst_r(ALU::sub_8, cpu.B);                   
    inst_table[0x91] = ALU_Inst_r(ALU::sub_8, cpu.C);       
    inst_table[0x92] = ALU_Inst_r(ALU::sub_8, cpu.D);         
    inst_table[0x93] = ALU_Inst_r(ALU::sub_8, cpu.E);       
    inst_table[0x94] = ALU_Inst_r(ALU::sub_8, cpu.H);       
    inst_table[0x95] = ALU_Inst_r(ALU::sub_8, cpu.L);      
    inst_table[0x96] = ALU_Inst_m(ALU::sub_8);           
    inst_table[0x97] = ALU_Inst_r(ALU::sub_8, cpu.A);    
    inst_table[0x98] = ALU_Inst_r(ALU::sbc_8, cpu.B);      
    inst_table[0x99] = ALU_Inst_r(ALU::sbc_8, cpu.C);     
    inst_table[0x9A] = ALU_Inst_r(ALU::sbc_8, cpu.D);         
    inst_table[0x9B] = ALU_Inst_r(ALU::sbc_8, cpu.E);         
    inst_table[0x9C] = ALU_Inst_r(ALU::sbc_8, cpu.H);          
    inst_table[0x9D] = ALU_Inst_r(ALU::sbc_8, cpu.L);         
    inst_table[0x9E] = ALU_Inst_m(ALU::sbc_8);             
    inst_table[0x9F] = ALU_Inst_r(ALU::sbc_8, cpu.A);

    inst_table[0xA0] = ALU_Inst_r(ALU::and_8, cpu.B);                   
    inst_table[0xA1] = ALU_Inst_r(ALU::and_8, cpu.C);       
    inst_table[0xA2] = ALU_Inst_r(ALU::and_8, cpu.D);         
    inst_table[0xA3] = ALU_Inst_r(ALU::and_8, cpu.E);       
    inst_table[0xA4] = ALU_Inst_r(ALU::and_8, cpu.H);       
    inst_table[0xA5] = ALU_Inst_r(ALU::and_8, cpu.L);      
    inst_table[0xA6] = ALU_Inst_m(ALU::and_8);           
    inst_table[0xA7] = ALU_Inst_r(ALU::and_8, cpu.A);    
    inst_table[0xA8] = ALU_Inst_r(ALU::xor_8, cpu.B);      
    inst_table[0xA9] = ALU_Inst_r(ALU::xor_8, cpu.C);     
    inst_table[0xAA] = ALU_Inst_r(ALU::xor_8, cpu.D);         
    inst_table[0xAB] = ALU_Inst_r(ALU::xor_8, cpu.E);         
    inst_table[0xAC] = ALU_Inst_r(ALU::xor_8, cpu.H);          
    inst_table[0xAD] = ALU_Inst_r(ALU::xor_8, cpu.L);         
    inst_table[0xAE] = ALU_Inst_m(ALU::xor_8);             
    inst_table[0xAF] = ALU_Inst_r(ALU::xor_8, cpu.A);

    inst_table[0xB0] = ALU_Inst_r(ALU::or_8, cpu.B);                   
    inst_table[0xB1] = ALU_Inst_r(ALU::or_8, cpu.C);       
    inst_table[0xB2] = ALU_Inst_r(ALU::or_8, cpu.D);         
    inst_table[0xB3] = ALU_Inst_r(ALU::or_8, cpu.E);       
    inst_table[0xB4] = ALU_Inst_r(ALU::or_8, cpu.H);       
    inst_table[0xB5] = ALU_Inst_r(ALU::or_8, cpu.L);      
    inst_table[0xB6] = ALU_Inst_m(ALU::or_8);           
    inst_table[0xB7] = ALU_Inst_r(ALU::or_8, cpu.A);    
    inst_table[0xB8] = ALU_Inst_r(ALU::cp_8, cpu.B);      
    inst_table[0xB9] = ALU_Inst_r(ALU::cp_8, cpu.C);     
    inst_table[0xBA] = ALU_Inst_r(ALU::cp_8, cpu.D);         
    inst_table[0xBB] = ALU_Inst_r(ALU::cp_8, cpu.E);         
    inst_table[0xBC] = ALU_Inst_r(ALU::cp_8, cpu.H);          
    inst_table[0xBD] = ALU_Inst_r(ALU::cp_8, cpu.L);         
    inst_table[0xBE] = ALU_Inst_m(ALU::cp_8);             
    inst_table[0xBF] = ALU_Inst_r(ALU::cp_8, cpu.A);

    inst_table[0xC0] = RET_IF(NZ);
    inst_table[0xC1] = POP_rr(cpu.B, cpu.C);
    inst_table[0xC2] = JP(NZ);
    inst_table[0xC3] = JP(no_cond);
    inst_table[0xC4] = CALL(NZ);
    inst_table[0xC5] = PUSH_rr(cpu.B, cpu.C);
    inst_table[0xC6] = ALU_Inst_n(ALU::add_8);
    inst_table[0xC7] = RST(0x00);
    inst_table[0xC8] = RET_IF(Z);
    inst_table[0xC9] = RET();
    inst_table[0xCA] = JP(Z);
    inst_table[0xCB] = PREFIX();
    inst_table[0xCC] = CALL(Z);
    inst_table[0xCD] = CALL(no_cond);
    inst_table[0xCE] = ALU_Inst_n(ALU::adc_8);
    inst_table[0xCF] = RST(0x08);

    inst_table[0xD0] = RET_IF(NC);
    inst_table[0xD1] = POP_rr(cpu.D, cpu.E);
    inst_table[0xD2] = JP(NC);
    inst_table[0xD3] = NOP();
    inst_table[0xD4] = CALL(NC);
    inst_table[0xD5] = PUSH_rr(cpu.D, cpu.E);
    inst_table[0xD6] = ALU_Inst_n(ALU::sub_8);
    inst_table[0xD7] = RST(0x10);
    inst_table[0xD8] = RET_IF(C);
    inst_table[0xD9] = RETI();
    inst_table[0xDA] = JP(C);
    inst_table[0xDB] = NOP();  
    inst_table[0xDC] = CALL(C);
    inst_table[0xDD] = NOP();
    inst_table[0xDE] = ALU_Inst_n(ALU::sbc_8);
    inst_table[0xDF] = RST(0x18);

    inst_table[0xE0] = LDH_n_A();
    inst_table[0xE1] = POP_rr(cpu.H, cpu.L);
    inst_table[0xE2] = LDH_C_A();
    inst_table[0xE3] = NOP();
    inst_table[0xE4] = NOP();
    inst_table[0xE5] = PUSH_rr(cpu.H, cpu.L);
    inst_table[0xE6] = ALU_Inst_n(ALU::and_8);
    inst_table[0xE7] = RST(0x20);
    inst_table[0xE8] = ADD_SPe();
    inst_table[0xE9] = JPHL();
    inst_table[0xEA] = LD_a16_A();
    inst_table[0xEB] = NOP();  
    inst_table[0xEC] = NOP();
    inst_table[0xED] = NOP();
    inst_table[0xEE] = ALU_Inst_n(ALU::xor_8);
    inst_table[0xEF] = RST(0x28);

    inst_table[0xF0] = LDH_A_n();
    inst_table[0xF1] = POP_AF();
    inst_table[0xF2] = LDH_A_C();
    inst_table[0xF3] = DI();
    inst_table[0xF4] = NOP();
    inst_table[0xF5] = PUSH_rr(cpu.A, cpu.F);
    inst_table[0xF6] = ALU_Inst_n(ALU::or_8);
    inst_table[0xF7] = RST(0x30);
    inst_table[0xF8] = LD_HL_SPe();
    inst_table[0xF9] = LD_SP_HL();
    inst_table[0xFA] = LD_A_a16();
    inst_table[0xFB] = EI();  
    inst_table[0xFC] = NOP();
    inst_table[0xFD] = NOP();
    inst_table[0xFE] = ALU_Inst_n(ALU::cp_8);
    inst_table[0xFF] = RST(0x38);
}

void Decoder::init_cb_table(CPU& cpu) {
    using namespace Arithmetic;

    uint8_t* reg;
    for(int i = 0; i < 0x08; ++i) {
        reg = regs[i % 8];  
        if(reg) cb_table[i] = ROT_Inst_r(rot_left_circ, *reg);
        else    cb_table[i] = ROT_Inst_m(rot_left_circ);
    }
    for(int i = 0x08; i < 0x10; ++i) {
        reg = regs[i % 8];
        if(reg) cb_table[i] = ROT_Inst_r(rot_right_circ, *reg);
        else    cb_table[i] = ROT_Inst_m(rot_right_circ);
    }
    for(int i = 0x10; i < 0x18; ++i) {
        reg = regs[i % 8];
        if(reg) cb_table[i] = ROT_Inst_r(rot_left, *reg);
        else    cb_table[i] = ROT_Inst_m(rot_left);
    }
    for(int i = 0x18; i < 0x20; ++i) {
        reg = regs[i % 8];
        if(reg) cb_table[i] = ROT_Inst_r(rot_right, *reg);
        else    cb_table[i] = ROT_Inst_m(rot_right);
    }
    for(int i = 0x20; i < 0x28; ++i) {
        reg = regs[i % 8];
        if(reg) cb_table[i] = ROT_Inst_r(shift_left_arithmetic, *reg);
        else    cb_table[i] = ROT_Inst_m(shift_left_arithmetic);
    }
    for(int i = 0x28; i < 0x30; ++i) {
        reg = regs[i % 8];
        if(reg) cb_table[i] = ROT_Inst_r(shift_right_arithmetic, *reg);
        else    cb_table[i] = ROT_Inst_m(shift_right_arithmetic);
    }
    for(int i = 0x30; i < 0x38; ++i) {
        reg = regs[i % 8];
        if(reg) cb_table[i] = SWAP_r(*reg);
        else    cb_table[i] = SWAP_m();
    }
    for(int i = 0x38; i < 0x40; ++i) {
        reg = regs[i % 8];
        if(reg) cb_table[i] = ROT_Inst_r(shift_right_logical, *reg);
        else    cb_table[i] = ROT_Inst_m(shift_right_logical);
    }
    for(int i = 0x40; i < 0x80; ++i) {
        reg = regs[i % 8];
        if(reg) cb_table[i] = BIT_r(*reg, (i - 0x40) / 8);
        else    cb_table[i] = BIT_m((i - 0x40) / 8);
    }
    for(int i = 0x80; i < 0xC0; ++i) {
        reg = regs[i % 8];
        if(reg) cb_table[i] = RES_r(*reg, (i - 0x80) / 8);
        else    cb_table[i] = RES_m((i - 0x80) / 8);
    }
    for(int i = 0xC0; i < 0x100; ++i) {
        reg = regs[i % 8];
        if(reg) cb_table[i] = SET_r(*reg, (i - 0xC0) / 8);
        else    cb_table[i] = SET_m((i - 0xC0) / 8);
    }
}

void Decoder::init_ivt(CPU& cpu) {
    ivt[static_cast<uint8_t>(Interrupt::VBLANK)]    = ISR(VBLANK_ISR);
    ivt[static_cast<uint8_t>(Interrupt::LCD)]       = ISR(LCD_ISR);
    ivt[static_cast<uint8_t>(Interrupt::TIMER)]     = ISR(TIMER_ISR);
    ivt[static_cast<uint8_t>(Interrupt::SERIAL)]    = ISR(SERIAL_ISR);
    ivt[static_cast<uint8_t>(Interrupt::JOYPAD)]    = ISR(JOYPAD_ISR);
}