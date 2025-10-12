#ifndef PIXELFETCHER_H
#define PIXELFETCHER_H

#include <cstdint>
#include <string>
#include <iostream>
#include "Tile.h"
#include "RingBuffer.h"

class Tile;
class VRAM;
class PPURegs;

class PixelFetcher {
//The fetcher iterates over each scanline, 
//finding the tile its on, and fetching the appropriate tile row
private:
    //data
    std::array<uint8_t, 8> px_buf;
    uint8_t x_pos, y_pos;           //current pos wrt tilemap origin
    Tile tile_data;                 //current tile data
    uint8_t tile_index;             //index of current tile 
    uint8_t cycles;

    bool stop_pending;
    bool on;

    //storage access
    const VRAM& vram;      
    const PPURegs& regs; 
    BgFifo& fifo;

public:
    PixelFetcher(const VRAM& vram, const PPURegs& control, BgFifo& fifo);
    using StateFunction = void(PixelFetcher::*)();
    //behavior
    enum class Mode {BG_FETCH, WIN_FETCH};
    enum class State {INIT, GET_ID, GET_TILE, GET_LINE, PUSH, PAUSING};

    void tick();

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
    void start();
    void request_stop();    

    //dbg stuff
    void print_state();

    //getters/setters
    bool active() const {return on;}

public:
    StateFunction curr_state;
    State curr_state_enum;
    Mode curr_mode;
};  

#endif