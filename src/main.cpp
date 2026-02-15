#include "Memory/MMU.h"
#include "Memory/MemoryMap.h"
#include "CPU.h"
#include "Disassembler.h"
#include "Graphics/PPU.h"
#include "Control/JoyPad.h"
#include "Control/InputHandler.h"
#include "Graphics/LCD.h"    
#include "Memory/Spaces.h"   
#include "Timer.h" 
#include "Memory/Bus.h" 
#include <iostream>

constexpr unsigned long FRAME_CYCLES = 17556;

int main(int argc, char* argv[]) {
    Bus bus;
    MMU mmu(bus);
    MemoryMap map(mmu);
    CPU cpu(bus, mmu, map.interrupt_controller);
    PPU ppu(bus, mmu, map.interrupt_controller);
    JoyPad jp(bus, mmu, map.interrupt_controller);
    InputHandler ih;
    Timer tim(bus, mmu, map.interrupt_controller);
    LCD display(3);

    std::string cart = argv[1];
    std::string filename = "../ROM/" + cart + ".gb";

    map.rom.load(filename);
    ppu.connect_display(&display);
    jp.connect_input_handler(&ih);

    SDL_Event e;
    bool quit = false;

    unsigned long next_frame_target = FRAME_CYCLES;

    while(!quit) {
        while(bus.get_cycles() < next_frame_target) {
            cpu.tick();
        }

        next_frame_target += FRAME_CYCLES;

        jp.read_input();
        display.draw_frame();

        while(SDL_PollEvent(&e)) {
            if(e.type == SDL_QUIT) {
                quit = true;
            }
        }
    }

    return 0;
}