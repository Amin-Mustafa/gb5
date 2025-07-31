#include "../include/MMU.h"
#include "../include/ROM.h"
#include "../include/MemoryContainer.h"
#include "../include/Register.h"
#include "../include/SerialPort.h"

#include "../include/CPU.h"
#include "../include/Disassembler.h"
#include <iostream>
#include <fstream>
#include <algorithm>

void dbg_out(SerialPort* sp) {
    for(const auto& c : sp->destination_buffer) {
        std::cout << (char)c;
    }
    std::cout << '\n';
}

int main() {
    MMU mem;
    ROM rom(mem);
    MemoryContainer ram(0x8000, 0xFFFF, mem);
    mem.write(0xFF44, 0x90);    //assume for now that LY = 0x90
    SerialPort sp(mem);
    CPU cpu(mem);
    Disassembler dis(mem);

    rom.load("ROM/09-op r,r.gb");
    while(true) {
        dis.disassemble_at(cpu.pc);
        cpu.print_state();
        dbg_out(&sp);
        cpu.tick();
        std::cin.get();
    }
}