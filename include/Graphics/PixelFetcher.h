#ifndef PIXELFETCHER_H
#define PIXELFETCHER_H

#include <cstdint>
#include <string>
#include <iostream>
#include "Tile.h"

class Tile;
class VRAM;
class FIFO;
class PPURegs;

class PixelFetcher {
//The fetcher iterates over each scanline, 
//finding the tile its on, and fetching the appropriate tile row
private:
    //data
    std::array<uint8_t, 8> queue;
    uint8_t x_pos, y_pos;  //current pos wrt tilemap origin
    Tile tile_data;                 //current tile data
    uint8_t tile_index;             //index of current tile 
    uint8_t cycles;

    //storage access
    const VRAM& vram;      
    const PPURegs& regs; 
    FIFO& fifo;

public:
    PixelFetcher(const VRAM& vram, const PPURegs& control, FIFO& fifo);
    using StateFunction = void(PixelFetcher::*)();
    //behavior
    enum class Mode {BG_FETCH, WIN_FETCH};

    void tick() {
        (this->*curr_state)();
        cycles++;
    }

    //state functions
    void init();    //fetcher takes 6 dots to wake up
    void get_tile_index();
    void get_tile();
    void get_tile_line();
    void push_to_fifo();

    //control
    void reset_fetch();
    void set_mode(Mode mode);
    void set_position(uint8_t x, uint8_t y);

    //dbg stuff
    void print_state();

private:
    StateFunction curr_state;
    Mode curr_mode;
};  

#endif