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

    std::string cart = "../ROM/test.gb";

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
            } else if (e.type == SDL_KEYDOWN) {
                switch (e.key.keysym.sym) {
                    case SDLK_SPACE:    //step one dot
                        ppu.print_state();
                        ppu.tick();
                        cycles++;
                        display.draw_frame();
                        if ((cycles % 4) == 0) {
                            cpu.tick();
                            mem.tick();
                        }
                        if (cycles == SCANLINE_DOTS) {
                            frame_count++;
                            cycles = 0;
                        }
                        break;
                    case SDLK_RETURN:  //step one scanline
                        for (size_t i = 0; i < SCANLINE_DOTS; i++) {
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
                        break;
                    case SDLK_f:  //step one frame
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
                        break;
                    case SDLK_ESCAPE:
                        quit = true;
                        break;
                }
            }
        }
        SDL_Delay(1);
    }
    return 0;
}