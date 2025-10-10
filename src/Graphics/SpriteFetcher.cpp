#include "../../include/Graphics/SpriteFetcher.h"
#include "../../include/Graphics/VRAM.h"   
#include "../../include/Graphics/PPURegs.h"   
#include "../../include/Memory/Spaces.h"   
#include <iostream>
#include <format>

constexpr int GET_ID_START = 0;
constexpr int GET_ROW_START = 1;
constexpr int GET_LINE_START = 4;
constexpr int PUSH_START = 5;
constexpr int SCREEN_Y_OFFSET = 16;
constexpr int BASE_SPR_HEIGHT = 8;

std::string sprite_info(const Sprite& spr);
std::string spr_fetcher_state_to_str(int cycles);

void SpriteFetcher::start() {
    on = true;
    cycles = GET_ID_START;
    curr_state = get_tile_index;

    (this->*curr_state)();
    cycles++;
}

void SpriteFetcher::stop() {
    //curr_spr = nullptr;
    cycles = GET_ID_START;
    curr_state = get_tile_index;

    on = false;
}

void SpriteFetcher::tick()  {
    if(!on) {
        return;
    }
    (this->*curr_state)();
    cycles++;
}
void SpriteFetcher::get_tile_index() {
    tile_index = curr_spr->index();

    curr_state = get_row;
}

void SpriteFetcher::get_row() {
    using LCDC::obj_size;

    if(cycles == GET_ROW_START) {
        uint8_t spr_height = BASE_SPR_HEIGHT + BASE_SPR_HEIGHT*(uint8_t)obj_size(regs);
        
        //the tile row is fixed here 
        row = regs.ly - curr_spr->y() + SCREEN_Y_OFFSET;
        if(curr_spr->y_flip()) {
            row = (spr_height-1) - row;  
        }

        //double height objects cause lower bit of tile index to be ignored
        if(spr_height == 2*BASE_SPR_HEIGHT) {
            tile_index &= 0xFE;
        }
    }
    //the tile data block is fixed. No need to do anything in second dot
    if(cycles == GET_LINE_START - 1) {
        curr_state = get_tile_line;
    }
    return;
}

void SpriteFetcher::get_tile_line() {
    //objects always use unsigned addressing
    Tile tile_data = vram.tile_at(tile_index, VRAM::AddressMode::UNSIGNED);
    for(size_t px = 0; px < px_buf.size(); ++px) {
        px_buf[px_buf.size()-1 - px] = tile_data.get_pixel(px, row);
    }

    curr_state = push_to_fifo;
}

void SpriteFetcher::push_to_fifo() {
    for(uint8_t px = 0; px < px_buf.size(); ++px) {
        SpritePixel spr_px {
            px_buf[px], (uint8_t)(curr_spr->x() + px), curr_spr->palette(), curr_spr->priority()
        };
        fifo.push(spr_px);
    }

    stop();
}

void SpriteFetcher::print_state() {
    std::cout << "Fetch state: ";
    if(!on) {
        std::cout << "INACTIVE";
    } else {
        std::cout << spr_fetcher_state_to_str(cycles);
    }
    std::cout << std::endl;

    //print the pixel queue
    std::cout << "Pixel buffer: ";
    for(const auto& n : px_buf) {
        std::cout << (int)n << ' ';
    }
    std::cout << "\t(cycle " << (int)cycles << ")";
    std::cout << std::endl;
    
    //print current sprite head
    std::cout << "Current Sprite: ";
    if(!curr_spr) {
        std::cout << "NULL";
    } else {
        std::cout << sprite_info(*curr_spr);
    }
    std::cout << std::endl;
}

std::string sprite_info(const Sprite& spr) {
    return std::format(
            "X:{:d}, Y:{:d}, INDEX:{:02x}", 
            spr.x(), 
            spr.y(),
            spr.index() 
        );
}

std::string spr_fetcher_state_to_str(int cycles) {
    if(cycles <= GET_ID_START) {
        return "GET ID";
    } else if(cycles <= GET_ROW_START) {
        return "GET ROW";
    } else if(cycles <= GET_LINE_START) {
        return "GET LINE";
    } else { 
        return "PUSH";
    }
}