#include "../include/Memory/MMU.h"
#include "../include/Memory/MemoryMap.h"
#include "../include/CPU.h"
#include "../include/Disassembler.h"
#include "../include/Graphics/PPU.h"
#include "../include/Graphics/LCD.h"    
#include "../include/Memory/Spaces.h"    
#include <iostream>

constexpr unsigned long FRAME_DOTS = 70224;

int main(int argc, char* argv[]) {
    MMU mem;
    MemoryMap map(mem);
    CPU cpu(mem, map.interrupt_controller);
    PPU ppu(mem, map.interrupt_controller);
    Disassembler dis(mem);
    LCD display(3);

    std::string cart = "../ROM/drmario.gb";

    map.rom.load(cart);
    ppu.connect_display(&display);

    SDL_Event e;
    bool quit = false;
    int cycles = 0;
    int step = FRAME_DOTS / 4;

    while(!quit) {
        //step 1 frame, dot by dot
        for(int i = 0; i < step; ++i) {
            cpu.tick();
            mem.tick();
            for(int c = 0; c < 4; ++c)
                ppu.tick();
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