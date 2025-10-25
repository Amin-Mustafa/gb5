#include "../include/Memory/MMU.h"
#include "../include/Memory/MemoryMap.h"
#include "../include/CPU.h"
#include "../include/Disassembler.h"
#include "../include/Graphics/PPU.h"
#include "../include/Graphics/LCD.h"    
#include "../include/Memory/Spaces.h"    
#include <iostream>

constexpr unsigned long FRAME_DOTS = 70224;
constexpr unsigned int SCANLINE_DOTS = 456;

int main(int argc, char* argv[]) {
    MMU mem;
    MemoryMap map(mem);
    CPU cpu(mem, map.interrupt_controller);
    PPU ppu(mem, map.interrupt_controller);
    Disassembler dis(mem);
    LCD display(3);

    std::string cart = "../ROM/01-special.gb";

    map.rom.load(cart);
    ppu.connect_display(&display);

    SDL_Event e;
    bool quit = false;
    unsigned long cycles = 0;
    unsigned long frame_count = 0;

    while(!quit) {
        while(SDL_PollEvent(&e)) {
            if(e.type == SDL_QUIT) {
                quit = true;
            }
        }
        for (size_t i = 0; i < FRAME_DOTS; i++) {
            ppu.tick();
            //ppu.print_state();
            cycles++;

            if ((cycles % 4) == 0) {
                cpu.tick();
                mem.tick();
            }
        }
        cycles = 0;
        display.draw_frame();
    }

    return 0;
}