#include "../../include/Graphics/PixelFetcher.h"
#include "../../include/Graphics/VRAM.h"
#include "../../include/Memory/Spaces.h"
#include "../../include/Graphics/RingBuffer.h"
#include "../../include/Graphics/Tile.h"
#include "../../include/Graphics/PPURegs.h"

#include <iostream>
#include <format>

//state starting cycles
constexpr int INIT_START = 0;
constexpr int GET_INDEX_START = 6;
constexpr int GET_TILE_START = 8;
constexpr int GET_LINE_START = 10;
constexpr int PUSH_START = 12;

std::string fetcher_state_to_str(PixelFetcher::State);

PixelFetcher::PixelFetcher(const VRAM& vram, const PPURegs& control, BgFifo& fifo) 
    : on{true},
      vram{vram}, regs{control}, fifo{fifo},
      curr_state{ init },
      curr_state_enum{State::INIT}
    {}

void PixelFetcher::tick() {
    if(!on) return;
    (this->*curr_state)();
    cycles++;
}

void PixelFetcher::init() {
    //do nothing for 6 dots
    if(cycles < GET_INDEX_START - 1) {
        return;
    }
    curr_state = get_tile_index;
    curr_state_enum = State::GET_ID;

    //wait for init to finish before checking stop
    if(stop_pending) {
        on = false;
    }
}

void PixelFetcher::get_tile_index() {
    if(cycles == GET_INDEX_START) {
        //idle for first cycle
        return;
    }

    //first step in fetch pipeline (2 dots)
    uint8_t tile_y = y_pos / 8;
    Space::TileMap map = Space::TILEMAP_0;  //by default

    switch(curr_mode) {
        case Mode::BG_FETCH:
            map = LCDC::bg_tilemap(regs) ? Space::TILEMAP_1 : Space::TILEMAP_0;
            break;
        case Mode::WIN_FETCH:
            map = LCDC::win_tilemap(regs) ? Space::TILEMAP_1 : Space::TILEMAP_0;
            break;
        default: break;
    } 

    tile_index = vram.read(map + tile_y*0x20 + x_pos);

    curr_state = get_tile;
    curr_state_enum = State::GET_TILE;

    if(stop_pending) {
        on = false;
    }
}

void PixelFetcher::get_tile() {
    //second step in fetch pipeline (2 dots)

    if(cycles == GET_TILE_START) {
        return;
    }

    using VRAM::AddressMode::SIGNED;
    using VRAM::AddressMode::UNSIGNED;
    VRAM::AddressMode mode = LCDC::bg_tile_area(regs) ? UNSIGNED : SIGNED;

    tile_data = vram.tile_at(tile_index, mode);

    curr_state = get_tile_line;
    curr_state_enum = State::GET_LINE;

    if(stop_pending) {
        on = false;
    }
}

void PixelFetcher::get_tile_line() {
    if(cycles == GET_LINE_START) {
        return;
    }

    //third step in fetch pipeline (2 dots)
    uint8_t row = y_pos % 8;    //row of tile
    for(size_t px = 0; px < px_buf.size(); ++px) {
        px_buf[px_buf.size()-1 - px] = tile_data.get_pixel(px, row);
    }

    curr_state = push_to_fifo;
    curr_state_enum = State::PUSH;

    if(stop_pending) {
        on = false;
    }
}

void PixelFetcher::push_to_fifo(){
    if(fifo.empty()) {
        for(size_t px = 0; px < px_buf.size(); ++px) {
            fifo.push(px_buf[px]);
        }
        //push successful
        //advance to next tile
        x_pos++;
        reset_fetch();          //...go back to start
    }

    //otherwise, not yet empty; dont push
    return;
}

void PixelFetcher::reset_fetch() {
    cycles = GET_INDEX_START - 1;
    curr_state = get_tile_index;
    curr_state_enum = State::GET_ID;
    std::fill(px_buf.begin(), px_buf.end(), 0);
}

void PixelFetcher::set_mode(Mode mode) {
    curr_mode = mode;
    std::fill(px_buf.begin(), px_buf.end(), 0);
    curr_state = init;
    cycles = INIT_START;
    curr_state_enum = State::INIT;
}

void PixelFetcher::set_position(uint8_t x, uint8_t y) {
    //convert screen-space x and y to tile x and y
    //transform from screen-space to bg or window space
    switch(curr_mode) {
        case Mode::BG_FETCH:
            x_pos = (x + regs.scx / 8) & 0x1F;
            y_pos = (y + regs.scy) & 0xFF;
            break;
        case Mode::WIN_FETCH:
            x_pos = 0;
            y_pos = y - regs.wy;
            break;

        default: break;
    }
}

void PixelFetcher::start() {
    //switch on
    on = true;
    stop_pending = false;
    (this->*curr_state)();
    cycles++;
}

void PixelFetcher::request_stop() {
    if(curr_state_enum == State::PUSH) {
        on = false; //stop right away
    } else {
        //wait until done with VRAM bus
        stop_pending = true;
    }
}

void PixelFetcher::print_state() {
    //print position
    std::cout << "Position: (" 
              << (int)x_pos << ", "<<(int)y_pos 
              << ")\n";
    //print current state
    std::cout << "Fetch state: ";
    if(!on) {
        std::cout << "INACTIVE";
    } else {
        std::cout << fetcher_state_to_str(curr_state_enum);
    }
    std::cout << std::endl;

    //print the pixel queue
    std::cout << "Fetch queue: ";
    for(const auto& n : px_buf) {
        std::cout << (int)n << ' ';
    } 
    std::cout << "\t(cycle " << (int)cycles << ")";
    std::cout << std::endl;
}

std::string fetcher_state_to_str(PixelFetcher::State state) {
    switch(state) {
        case PixelFetcher::State::INIT:
            return "INIT";
        case PixelFetcher::State::GET_ID:
            return "GET ID";
        case PixelFetcher::State::GET_TILE:
            return "GET TILE";
        case PixelFetcher::State::GET_LINE:
            return "GET LINE";
        case PixelFetcher::State::PUSH:
            return "PUSH";
        case PixelFetcher::State::PAUSING:
            return "PAUSING";
    }
}