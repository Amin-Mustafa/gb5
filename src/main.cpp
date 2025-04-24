#include "../include/Instruction.h"
#include "../include/CPU.h"
#include "../include/MMU.h"
#include <iostream>

int main() {
    SM83::MemoryMap map;
    SM83::MMU mem(map);
    SM83::CPU cpu(mem);
}