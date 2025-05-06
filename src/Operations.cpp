#include "../include/Instruction.h"
#include "../include/CPU.h"

template<typename T>
constexpr T& get_ref(SM83::Operand op) {
    return op.get<std::reference_wrapper<T>>().get();
}

template<typename T>
constexpr T get_val(SM83::Operand op) {
    op.get<T>();
}

namespace SM83 {

//Basic operations
void nop(Instruction::Args& args){
    /* do nothing */
}

void load8(Instruction::Args& args) {
    get_ref<uint8_t>(args[0]) = get_val<uint8_t>(args[1]);
}


}