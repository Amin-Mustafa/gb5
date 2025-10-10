#ifndef SPRITEFETCHER_H
#define SPRITEFETCHER_H

#include <cstdint>
#include <array>
#include <vector>
#include "RingBuffer.h"
#include "Sprite.h"
#include "Tile.h"

class VRAM;
class PPURegs;

class SpriteFetcher {
private:
    std::array<uint8_t, 8> px_buf;
    const Sprite* curr_spr = nullptr;

    //storage access
    const VRAM& vram;      
    const PPURegs& regs; 
    RingBuffer<SpritePixel, 16>& fifo;

    uint8_t row;
    uint8_t tile_index;
    
    //FSM behavior
    bool on = false;    //the spr fifo only works periodically
    int cycles;
public: 
    SpriteFetcher(const VRAM& vram, const PPURegs& control, RingBuffer<SpritePixel, 16>& sprite_fifo)
        : vram{vram},
          regs{control},
          fifo{sprite_fifo},
          curr_state{get_tile_index}
          {}

    //getter/setter
    bool active() const {
        return on;
    }
    void set_sprite(const Sprite& spr) {
        curr_spr = &spr;
    }

    //control
    void start();
    void stop();
    void tick();

    //state functions
    using StateFunction = void(SpriteFetcher::*)();
    void get_tile_index();
    void get_row();
    void get_tile_line();
    void push_to_fifo();

    //dbg stuff
    void print_state();

private:
    StateFunction curr_state;
};

#endif