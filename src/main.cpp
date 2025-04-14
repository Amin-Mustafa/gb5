#include "../include/Instruction.h"
#include <iostream>

void print_message(std::vector<unsigned char>& args) {
    std::cout << "Yeah buddy!\n";
}

int main() {
    GBA::Instruction inst{print_message, {}, "PRINT"};
    inst.print();
    inst.execute();
}