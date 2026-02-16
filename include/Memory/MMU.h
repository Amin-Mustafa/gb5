#ifndef MMU_H
#define MMU_H

#include <vector>
#include <cstdint>
#include <memory>
#include <array>
#include "IO.h"
#include "Spaces.h"

class Bus;

class MMU {
private:   
    std::array<uint8_t*, 0x100> pages;
    std::array<IO*, 0x100> io_registers;
    std::array<uint8_t, 127> hram;
public:
    MMU(Bus& bus);
    ~MMU();
    void map_region(uint16_t start, uint16_t end, uint8_t* data);
    void unmap_region(uint16_t start, uint16_t end);
    void map_io_region(uint16_t start, uint16_t end, IO* io_reg);
    void map_io_register(uint16_t addr, IO* io_reg);
    uint8_t read(uint16_t addr);
    void write(uint16_t addr, uint8_t val);
};

#endif