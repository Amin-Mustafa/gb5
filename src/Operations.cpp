#include "../include/Instruction.h"
#include "../include/CPU.h"

namespace SM83 {

//Basic operations

void nop(Instruction::Args& args){

}

void load8(Instruction::Args& args) {
    args[0].get() = args[1].get();
}


}