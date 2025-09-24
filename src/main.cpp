#include "../include/Memory/MMU.h"
#include "../include/Memory/MemoryMap.h"
#include "../include/CPU.h"
#include "../include/Disassembler.h"
#include "../include/Graphics/PPU.h"
#include <iostream>

int main(int argc, char* argv[]) {
    MMU mem;
    MemoryMap map(mem);
    CPU cpu(mem, map.interrupt_controller);
    PPU ppu(mem, map.interrupt_controller);
    Disassembler dis(mem);
    LCD display(3);

    std::string cart = "../ROM/test.gb";

    map.rom.load(cart);
    ppu.connect_display(&display);

    SDL_Event e;
    bool quit = false;

    while(!quit) {
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT) {
                quit = true;
            } else if (e.type == SDL_KEYDOWN && e.key.keysym.sym == SDLK_RETURN) {
                // step one PPU cycle
                ppu.print_state();
                ppu.tick();
                display.draw_frame();
            }
        }
    }
}