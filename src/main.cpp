#include "../include/Memory/MMU.h"
#include "../include/Memory/MemoryMap.h"
#include "../include/CPU.h"
#include "../include/Disassembler.h"
#include "../include/Graphics/PPU.h"
#include "../include/Graphics/LCD.h"    
#include "../include/Graphics/Spaces.h"    
#include <iostream>

constexpr unsigned long FRAME_DOTS = 70224;

int main(int argc, char* argv[]) {
    MMU mem;
    MemoryMap map(mem);
    CPU cpu(mem, map.interrupt_controller);
    PPU ppu(mem, map.interrupt_controller);
    Disassembler dis(mem);
    LCD display(3);

    std::string cart = "../ROM/05-op rp.gb";

    map.rom.load(cart);
    ppu.connect_display(&display);

    SDL_Event e;
    bool quit = false;
    int cycles = 0;
    int step = FRAME_DOTS;

    while(!quit) {
        //step 1 frame, dot by dot
        for(int i = 0; i < step; ++i) {
            ppu.tick();
            cycles++;
            if(cycles == 4) {
                //tick cpu every 4 dots
                cycles = 0;
                cpu.tick();
            }
        }
        display.draw_frame();

        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT) {
                quit = true;
            }
        }
    }

    return 0;
}