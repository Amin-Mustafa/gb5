#ifndef MEMORYMAP_H
#define MEMORYMAP_H

#include <memory>
#include <string>

#include "ROM.h"
#include "MemoryContainer.h"
#include "SerialPort.h"
#include "InterruptController.h"

class MMU;

struct MemoryMap {
    ROM rom;
    MemoryContainer ram;
    SerialPort serial_port;
    InterruptController interrupt_controller;

    MemoryMap(MMU& mmu) 
        :rom{mmu},
         ram{mmu, 0x8000, 0xFFFF}, 
         serial_port{mmu}, 
         interrupt_controller{mmu} {}
};

#endif