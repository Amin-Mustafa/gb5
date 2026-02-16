#ifndef BUS_H
#define BUS_H

#include <cstdint>
#include "DmaController.h"

class MMU;
class CPU;
class PPU;
class JoyPad;
class Timer;
class OAM;

class Bus {
private:
    MMU* mmu;
    CPU* cpu;
    PPU* ppu;
    JoyPad* jp;
    Timer* tim;

    DmaController dmac;

    unsigned long cycles = 0;
public:
    void connect(MMU& Mmu)   {mmu = &Mmu;}
    void connect(CPU& Cpu)   {cpu = &Cpu;} 
    void connect(PPU& Ppu)   {ppu = &Ppu;}  
    void connect(JoyPad& Jp) {jp  = &Jp;}  
    void connect(Timer& Tim) {tim = &Tim;}  

    void cycle();
    uint8_t read(uint16_t addr);
    void write(uint16_t addr, uint8_t val);

    unsigned long get_cycles() const {return cycles;}

    //dma functions
    OAM* oam_dma_dest;
    void start_dma(uint8_t page);
    bool dma_active() const {return dmac.active();}
};

inline bool addr_in_hram(uint16_t addr) {
    return (0xFF80 <= addr) && (addr <= 0xFFFE);
}

#endif