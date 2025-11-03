#include "../include/Memory/MMU.h"
#include "../include/Memory/MemoryMap.h"
#include "../include/CPU.h"
#include "../include/Disassembler.h"
#include "../include/Graphics/PPU.h"
#include "../include/Control/JoyPad.h"
#include "../include/Control/InputHandler.h"
#include "../include/Graphics/LCD.h"    
#include "../include/Memory/Spaces.h"   
#include "../include/Timer.h" 
#include <iostream>

constexpr unsigned long FRAME_DOTS = 70224;
constexpr unsigned int SCANLINE_DOTS = 456;

int main(int argc, char* argv[]) {
    MMU mem;
    MemoryMap map(mem);
    CPU cpu(mem, map.interrupt_controller);
    PPU ppu(mem, map.interrupt_controller);
    JoyPad jp(mem, map.interrupt_controller);
    InputHandler ih;
    Timer tim(mem, map.interrupt_controller);
    Disassembler dis(mem);
    LCD display(3);

    std::string cart = "../ROM/drmario.gb";

    map.rom.load(cart);
    ppu.connect_display(&display);
    jp.connect_input_handler(&ih);

    SDL_Event e;
    bool quit = false;
    unsigned long cycles = 0;
    unsigned long frame_count = 0;

    while(!quit) {
        for (size_t i = 0; i < FRAME_DOTS; i++) {
            ppu.tick();
            tim.tick();
            jp.read_input();
            cycles++;

            if ((cycles % 4) == 0) {
                cpu.tick();
                mem.tick();
            }
        }

        cycles = 0;
        display.draw_frame();

        while(SDL_PollEvent(&e)) {
            if(e.type == SDL_QUIT) {
                quit = true;
            }
        }
    }

    return 0;
}