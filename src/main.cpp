#include "../include/Memory/MMU.h"
#include "../include/Register.h"
#include "../include/Memory/MemoryMap.h"

#include "../include/CPU.h"
#include "../include/Disassembler.h"
#include <iostream>
#include <fstream>
#include <algorithm>

std::string dbg_out(SerialPort& sp) {
    std::string dbg_msg;
    for(const auto& c : sp.destination_buffer) {
        dbg_msg += c;
        std::cout << (char)c;
    }
    std::cout << '\n';
    return dbg_msg;
}

long line_count(const std::string& filename) {
    std::fstream fs;
    fs.open(filename);
    auto count = std::count_if(std::istreambuf_iterator<char>{fs}, {}, [](char c) { return c == '\n'; });
    return count;
    fs.close();
}

int main(int argc, char* argv[]) {
    MMU mem;
    MemoryMap map(mem);
    CPU cpu(mem, map.interrupt_controller);
    Disassembler dis(mem);

    std::string cart = "../ROM/07-jr,jp,call,ret,rst.gb";

    std::string blargg_log_file = "../log_cmp/Blargg.txt";
    std::string my_log_file = "../log_cmp/log.txt";
    std::ofstream fs;
    long blargg_log_lines;
    std::string option = argv[1];

    mem.write(0xFF44, 0x90);    //assume for now that LY = 0x90
    map.rom.load(cart);
    
    if(option == "log") {
        fs.open(my_log_file);
        blargg_log_lines = line_count(blargg_log_file);
        std::cout << blargg_log_lines << '\n';
        for(long lines = 0; lines <= blargg_log_lines; ++lines) {
            cpu.log_state(fs);
            cpu.tick();
        }
        fs.close(); 
    }

    else if(option == "dis") {
        while(true) {
            dis.disassemble_at(cpu.pc);
            cpu.print_state();
            cpu.tick();
            dbg_out(map.serial_port);
            std::cin.get();
        }   
    }
}