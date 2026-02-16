#ifndef CONSOLE_H
#define CONSOLE_H

#include "Memory/MMU.h"
#include "Memory/InterruptController.h"
#include "Memory/ROM.h"
#include "Memory/Bus.h"
#include "Memory/SerialPort.h"
#include "Graphics/PPU.h"
#include "Graphics/LCD.h"
#include "Control/Joypad.h"
#include "Control/InputHandler.h"
#include "CPU.h"
#include "Timer.h"

class Console {
public:
    //shared
    Bus bus;
    MMU mmu;
    InterruptController ic;

    //memory
    std::array<uint8_t, 0x2000> ext_ram;
    std::array<uint8_t, 0x2000> wram;

    //components
    ROM rom;
    CPU cpu;
    PPU ppu;
    JoyPad jp;
    Timer tim;

    //externals
    InputHandler ih;
    LCD display;

    Console() 
    :bus{}, 
     mmu{bus},    
     ic{mmu},     
          
     ext_ram{},     
     wram{},

     rom{mmu},
     cpu{bus, mmu, ic},
     ppu{bus, mmu, ic},
     jp{bus, mmu, ic},
     tim{bus, mmu, ic},

     ih{},
     display{3}
    {
        mmu.map_region(0xA000, 0xBFFF, ext_ram.data());
        mmu.map_region(0xC000, 0xDFFF, wram.data());
        mmu.map_region(0xE000, 0xFDFF, wram.data());    //echo ram

        ppu.connect_display(&display);
        jp.connect_input_handler(&ih);
    }
};

#endif