#ifndef MEMORYMAP_H
#define MEMORYMAP_H

#include <memory>
#include <string>

#include "../include/ROM.h"
#include "../include/MemoryContainer.h"
#include "../include/SerialPort.h"
#include "../include/InterruptHandler.h"

class MMU;

struct MemoryMap {
    ROM rom;
    MemoryContainer ram;
    SerialPort serial_port;
    InterruptHandler interrupt_handler;

    MemoryMap(MMU& mmu) 
        :rom{mmu}, ram{mmu, 0x8000, 0xFFFF}, serial_port{mmu}, interrupt_handler{mmu} {}
};

#endif