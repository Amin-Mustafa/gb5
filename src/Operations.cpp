#include "../include/Instruction.h"

namespace SM83 {

//Basic operations

void load8(Instruction::Args& args) {
    args[0].get() = args[1].get();
}

}