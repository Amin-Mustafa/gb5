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
bool px_occupied(SprFifo& fifo, SpritePixel candidate);

void SpriteFetcher::start() {
    on = true;
    cycles = GET_ID_START;
    curr_state = get_tile_index;

    (this->*curr_state)();
    cycles++;
}

void SpriteFetcher::stop() {
    //spr_queue.front() = nullptr;
    cycles = GET_ID_START;
    curr_state = get_tile_index;

    on = false;
}

void SpriteFetcher::reset_fetch() {
    cycles = GET_ID_START - 1;
    curr_state = get_tile_index;
    std::fill(px_buf.begin(), px_buf.end(), 0);
    
    on = true;
}

void SpriteFetcher::tick()  {
    if(!on) {
        return;
    }
    (this->*curr_state)();
    cycles++;
}
void SpriteFetcher::get_tile_index() {
    tile_index = spr_queue.front()->index();

    curr_state = get_row;
}

void SpriteFetcher::get_row() {
    using LCDC::obj_size;

    if(cycles == GET_ROW_START) {
        uint8_t spr_height = BASE_SPR_HEIGHT + BASE_SPR_HEIGHT*(uint8_t)obj_size(regs);
        
        //the tile row is fixed here 
        row = regs.ly - spr_queue.front()->y() + SCREEN_Y_OFFSET;
        if(spr_queue.front()->y_flip()) {
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
    SpritePixel spr_px;

    for(uint8_t px = 0; px < px_buf.size(); ++px) {
        spr_px.color = px_buf[px];
        spr_px.x = spr_queue.front()->x() + px;
        spr_px.palette = spr_queue.front()->palette();
        spr_px.priority = spr_queue.front()->priority();
        
        if(!px_occupied(fifo, spr_px)) {
            fifo.push(spr_px);
        }
    }
    spr_queue.pop();
    if(spr_queue.empty()) {
        stop();
    } else {
        reset_fetch();
    }
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
    if(!spr_queue.front()) {
        std::cout << "NULL";
    } else {
        std::cout << sprite_info(*spr_queue.front());
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

bool px_occupied(SprFifo& fifo, SpritePixel candidate) {
    bool occupied = false;
    for(SpritePixel& px : fifo) {
        if(px.x == candidate.x) {
            occupied = true;
            //position is occupied
            if(!px.color) {
                px = candidate; //replace if existing px transparent
            }
        }
    }
    return occupied;
}