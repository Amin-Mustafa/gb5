#include "../../include/Graphics/PixelFetcher.h"
#include "../../include/Graphics/VRAM.h"
#include "../../include/Memory/Spaces.h"
#include "../../include/Graphics/FIFO.h"
#include "../../include/Graphics/Tile.h"
#include "../../include/Graphics/PPURegs.h"

#include <iostream>
#include <format>

constexpr int INIT_CYCLES = 6;
constexpr int GET_INDEX_START = 0 + INIT_CYCLES;
constexpr int GET_TILE_START = 2 + INIT_CYCLES;
constexpr int GET_LINE_START = 4 + INIT_CYCLES;
constexpr int PUSH_START = 6 + INIT_CYCLES;

const std::string fetcher_state_to_str(uint8_t dot);

PixelFetcher::PixelFetcher(const VRAM& vram, const PPURegs& control, FIFO& fifo) 
    : vram{vram}, regs{control}, fifo{fifo},
      curr_state{ init }
    {}

void PixelFetcher::init() {
    //do nothing for 6 dots
    if(cycles == GET_INDEX_START) {
        curr_state = get_tile_index;
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
}

void PixelFetcher::get_tile() {
    if(cycles == GET_TILE_START) {
        return;
    }

    //second step in fetch pipeline (2 dots)
    using VRAM::AddressMode::SIGNED;
    using VRAM::AddressMode::UNSIGNED;
    VRAM::AddressMode mode = LCDC::bg_tile_area(regs) ? UNSIGNED : SIGNED;

    tile_data = vram.tile_at(tile_index, mode);

    curr_state = get_tile_line;
}

void PixelFetcher::get_tile_line() {
    if(cycles == GET_LINE_START) {
        return;
    }

    //third step in fetch pipeline (2 dots)
    uint8_t row = y_pos % 8;    //row of tile
    for(size_t px = 0; px < queue.size(); ++px) {
        queue[queue.size()-1 - px] = tile_data.get_pixel(px, row);
    }

    curr_state = push_to_fifo;
}

void PixelFetcher::push_to_fifo(){
    if(fifo.empty()) {
        for(size_t px = 0; px < queue.size(); ++px) {
            fifo.push(queue[px]);
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
    cycles = GET_INDEX_START;
    curr_state = get_tile_index;
    std::fill(queue.begin(), queue.end(), 0);
}

void PixelFetcher::set_mode(Mode mode) {
    curr_mode = mode;
    cycles = 0;
    std::fill(queue.begin(), queue.end(), 0);
    curr_state = init;
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

void PixelFetcher::print_state() {
    //print position
    std::cout << "Position: (" 
              << (int)x_pos << ", "<<(int)y_pos 
              << ")\n";
    //print current state
    std::cout << "Fetch state: " 
              << fetcher_state_to_str(cycles) 
              << std::endl;

    //print the pixel queue
    std::cout << "Fetch queue: ";
    for(const auto& n : queue) {
        std::cout << (int)n << ' ';
    } 
    std::cout << "\t(cycle " << (int)cycles << ")";
    std::cout << std::endl;
}

const std::string fetcher_state_to_str(uint8_t dot) {
    if(dot < GET_INDEX_START) 
        return "INIT";
    if(dot < GET_TILE_START) 
        return "GET INDEX";
    if(dot < GET_LINE_START)
        return "GET TILE";
    if(dot < PUSH_START)
        return "GET LINE";
    return "PUSH";
}