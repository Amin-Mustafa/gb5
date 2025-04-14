#include "../include/Instruction.h"
#include <iostream>
namespace GBA {

void Instruction::print() {
    if(_opname.length() > 0) {
        std::cout << _opname << '\t';
        for(const auto& a : _args) {
            std::cout << (int)a << ' ';
        }
        std::cout << '\n';
    }
}

void Instruction::execute() {
    if(_op) {
        _op(_args);
    }
    else std::cerr << "Null operation\n";
}

}