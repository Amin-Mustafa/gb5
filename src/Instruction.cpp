#include "../include/Instruction.h"
#include <iostream>

namespace SM83 {

void Instruction::print() {
    if(opname.length() > 0) {
        std::cout << opname << '\t';
        //for(const auto& arg: args){
            //printf("%02x ", (int)arg.get();
        //}
        std::cout << '\n';
    }
}

void Instruction::execute(CPU& cpu) {
    std::visit([&](auto&& f) {
        if (!f) {
            std::cerr << "Null operation\n";
            return;
        }

        using T = std::decay_t<decltype(f)>;
        if constexpr (std::is_same_v<T, Argfn>) {
            f(args);
        } else if constexpr (std::is_same_v<T, CPUfn>) {
            (cpu.*f)(); 
        }
    }, op);
}
}