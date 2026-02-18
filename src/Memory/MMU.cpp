#include "Memory/MMU.h"
#include "Memory/Bus.h"
#include "Memory/IO.h"
#include "MBC/MBC.h"
#include <stdexcept>
#include <iostream>

inline bool between(uint16_t num, uint16_t lo, uint16_t hi) {
    return (num >= lo) && (num <= hi);
}
bool reserved_address(uint16_t addr) {
    return between(addr, Space::OAM_RESERVED_START, Space::OAM_RESERVED_END);
}

MMU::MMU(Bus& bus)  
    {
        bus.connect(*this);
        std::fill(std::begin(pages), std::end(pages), nullptr);
        std::fill(std::begin(io_registers), std::end(io_registers), nullptr);
    }

MMU::~MMU() = default;

void MMU::map_region(uint16_t start, uint16_t end, uint8_t* data) {
    //map pages as byte arrays
    uint8_t start_page = start >> 8;
    uint8_t end_page   = end >> 8;
    for(auto i = start_page; i <= end_page; ++i) {
        uint16_t offset = (i - start_page) * 0x100; //address of current memory page
        pages[i] = data + offset;
    }
}

void MMU::unmap_region(uint16_t start, uint16_t end) {
    uint8_t start_page = start >> 8;
    uint8_t end_page   = end >> 8;
    for(auto i = start_page; i <= end_page; ++i) {
        pages[i] = nullptr;
    }
}

void MMU::map_io_register(uint16_t addr, IO* reg) {
    io_registers[addr & 0xFF] = reg;
}

void MMU::map_io_region(uint16_t start, uint16_t end, IO* io_reg) {
    for(auto i = start; i <= end; ++i) {
        io_registers[i & 0xFF] = io_reg;
    }
}

uint8_t MMU::read(uint16_t addr) {
    if(reserved_address(addr)) {
        return 0xFF;
    }
    //normal memory
    if(addr < 0xFF00) {
        uint8_t* data = pages[addr >> 8];
        if(data) {
            return data[addr & 0xFF];
        } else {
            //inaccessible memory; ignore read
            return 0xFF;    
        }
    }
    
    if (addr >= Space::HRAM_START && addr <= Space::HRAM_END) {
            return hram[addr & 0x7F];
    }

    IO* io_handler = io_registers[addr & 0xFF];
    if(io_handler) {
        return io_handler->read(addr);
    } else {
        //return 0xFF;    //unimplemented memory
        return fallback[addr];
    }
}

void MMU::write(uint16_t addr, uint8_t val) {
    if(reserved_address(addr)) {
        return;
    }

    if(addr < 0x8000) {
        if(mbc) mbc->write(addr, val);
        return;
    }
    if(addr < 0xFF00) {
        uint8_t* data = pages[addr >> 8];
        if(data) {
            data[addr & 0xFF] = val;
        }
        return; 
    }

    if(addr >= Space::HRAM_START && addr <= Space::HRAM_END) {
        hram[addr & 0x7F] = val;
        return;
    }

    IO* handler = io_registers[addr & 0xFF];
    if(handler) {
        handler->write(addr, val);
    } else {
        fallback[addr] = val;
    }
}