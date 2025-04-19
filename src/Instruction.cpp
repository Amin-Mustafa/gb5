#include "../include/Instruction.h"
#include <iostream>
namespace SM83 {

void Instruction::print() {
    if(_opname.length() > 0) {
        std::cout << _opname << '\t';
        for(const auto& arg: _args){
            std::cout << (int)arg.get() << ' ';
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