#include "Console.h"
#include <iostream>

constexpr unsigned long FRAME_CYCLES = 17556;

int main(int argc, char* argv[]) {
    Console gb;

    std::string cart = argv[1];
    std::string filename = "../ROM/" + cart + ".gb";

    gb.rom.load(filename);

    SDL_Event e;
    bool quit = false;

    unsigned long next_frame_target = FRAME_CYCLES;

    while(!quit) {
        while(gb.bus.get_cycles() < next_frame_target) {
            gb.cpu.tick();
        }

        next_frame_target += FRAME_CYCLES;

        gb.jp.read_input();
        gb.display.draw_frame();

        while(SDL_PollEvent(&e)) {
            if(e.type == SDL_QUIT) {
                quit = true;
            }
        }
    }

    return 0;
}