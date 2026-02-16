#include "Memory/Bus.h"    
#include "CPU.h"    
#include "Graphics/PPU.h"  
#include "Graphics/OAM.h"
#include "Memory/MMU.h"
#include "Control/Joypad.h"
#include "Timer.h"
#include "Memory/Spaces.h"  

void Bus::cycle() { 
    if(dmac.active()) {
        uint16_t src_addr = dmac.start_address() + dmac.offset();
        uint16_t dest_addr = Space::OAM_START + dmac.offset();
        uint8_t val = mmu->read(src_addr);
        oam_dma_dest->write(dest_addr, val);
        dmac.tick();
    }
    if(!ppu || !tim || !jp) return; 
    for(int i = 0; i < 4; ++i) {
        ppu->tick();
        tim->tick();
    }
    cycles++;
}

uint8_t Bus::read(uint16_t addr) {
    //reading the bus advances time
    cycle(); 
    if(!dmac.active() || addr_in_hram(addr)) {
        //dma blocks the bus
        return mmu->read(addr);
    } else {
        return 0xFF;    //garbage read
    }
}

void Bus::write(uint16_t addr, uint8_t val) {
    //writing to bus advances time
    cycle();
    if(addr == Space::DMA) {
        start_dma(val);
        mmu->write(addr, val);
        return;
    }
    if(!dmac.active() || addr_in_hram(addr)) {
        mmu->write(addr, val);
    }
}

void Bus::start_dma(uint8_t page) {
    uint16_t start_addr = (uint16_t)page << 8; 
    //start DMA
    dmac.start(start_addr);
}