#include "../include/Memory/MMU.h"
#include "../include/Memory/MemoryMap.h"

#include "../include/CPU.h"
#include "../include/Disassembler.h"
#include <iostream>

int main() {
    MMU mem;
    MemoryMap map(mem);
    CPU cpu(mem, map.interrupt_controller);
    Disassembler dis(mem);

    std::string cart = "../ROM/test.gb";

    map.rom.load(cart);

     while(true) {
        cpu.tick();
        std::cin.get();
     }
}