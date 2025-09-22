#include "../include/Memory/MMU.h"
#include "../include/Memory/MemoryMap.h"

#include "../include/CPU.h"
#include "../include/Disassembler.h"
#include "../include/Graphics/PPU.h"
#include <iostream>
#include <fstream>
#include <algorithm>

long line_count(const std::string& filename) {
    std::fstream fs;
    fs.open(filename);
    auto count = std::count_if(std::istreambuf_iterator<char>{fs}, {}, [](char c) { return c == '\n'; });
    return count;
    fs.close();
}

int main() {
    MMU mem;
    MemoryMap map(mem);
    CPU cpu(mem, map.interrupt_controller);
    PPU ppu(mem, map.interrupt_controller);
    Disassembler dis(mem);

    std::string cart = "../ROM/11-op a,(hl).gb";

    map.rom.load(cart);

    while(true) {
        //ppu.print_state();
        ppu.tick();
        std::cin.get();
    }   
}