#ifndef SPRITEFETCHER_H
#define SPRITEFETCHER_H

#include <cstdint>
#include <array>
#include <vector>
#include "RingBuffer.h"
#include "Sprite.h"
#include "Tile.h"
#include <iostream>
#include <format>

class VRAM;
class PPURegs;

class SpriteFetcher {
private:
    std::array<uint8_t, 8> px_buf;
    RingBuffer<Sprite, 8> spr_queue;

    //storage access
    const VRAM& vram;      
    const PPURegs& regs; 
    SprFifo& fifo;

    uint8_t row;
    uint8_t tile_index;
    
    //FSM behavior
    bool on = false;    //the spr fifo only works periodically
    int cycles = 0;
public: 
    SpriteFetcher(const VRAM& vram, const PPURegs& control, SprFifo& sprite_fifo)
        : vram{vram},
          regs{control},
          fifo{sprite_fifo},
          curr_state{get_tile_index}
          {}

    //getter/setter
    bool active() const {
        return on;
    }
    void queue_sprite(const Sprite& spr) {
        spr_queue.push(spr);
    }

    //control
    void start();
    void stop();
    void reset_fetch();
    void tick();
    void clear_queue() { spr_queue.clear(); }

    //state functions
    using StateFunction = void(SpriteFetcher::*)();
    void get_tile_index();
    void get_row();
    void get_tile_line();
    void push_to_fifo();

private:
    StateFunction curr_state;
};

#endif