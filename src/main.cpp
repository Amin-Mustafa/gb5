#include "../include/Memory/MMU.h"
#include "../include/Memory/MemoryMap.h"

#include "../include/CPU.h"
#include "../include/Disassembler.h"
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
    Disassembler dis(mem);

    std::string cart = "../ROM/11-op a,(hl).gb";

    mem.write(0xFF44, 0x90);    //assume for now that LY = 0x90
    map.rom.load(cart);

    while(true) {
        cpu.tick();
    }   
}