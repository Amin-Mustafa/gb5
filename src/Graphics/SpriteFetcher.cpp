#include "Graphics/SpriteFetcher.h"
#include "Graphics/VRAM.h"   
#include "Graphics/PPURegs.h"   
#include "Memory/Spaces.h"   
#include <iostream>
#include <format>

constexpr int GET_ID_START = 0;
constexpr int GET_ROW_START = 1;
constexpr int GET_LINE_START = 4;
constexpr int PUSH_START = 5;
constexpr int SCREEN_Y_OFFSET = 16;
constexpr int SCREEN_X_OFFSET = 8;
constexpr int BASE_SPR_HEIGHT = 8;

bool px_occupied(SprFifo& fifo, SpritePixel candidate);

void SpriteFetcher::start() {
    if(spr_queue.empty()) {
        on = false;
        return;
    }
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
    tile_index = spr_queue.front().index();

    curr_state = get_row;
}

void SpriteFetcher::get_row() {
    using LCDC::obj_size;

    if(cycles == GET_ROW_START) {
        uint8_t spr_height = BASE_SPR_HEIGHT + BASE_SPR_HEIGHT*(uint8_t)obj_size(regs);
        
        // 1. Calculate row within the full sprite (e.g., 0-15 for 8x16)
        row = regs.ly - spr_queue.front().y() + SCREEN_Y_OFFSET;
        if(spr_queue.front().y_flip()) {
            row = (spr_height-1) - row;  
        }

        // 2. Adjust Tile Index and Row for 8x16 mode
        if(spr_height == 16) {
            // Mask to top tile initially
            tile_index &= 0xFE; 
            
            // If we are in the bottom half of the sprite
            if(row >= 8) {
                tile_index |= 0x01; // Use the next tile
                row -= 8;           // Normalize row to 0-7
            }
        }
        // For 8x8, row is already 0-7 and index is correct
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
    Sprite spr = spr_queue.pop();
    //color index from vram, attributes from spr
    SpritePixel spr_px;

    bool reversed = spr.x_flip();
    int start, end, step;

    if(reversed) {
        start = px_buf.size() - 1;
        end   = -1; 
        step  = -1;
    } else {
        start = 0;
        end   = px_buf.size();
        step  = 1;
    }

    for(int i = start; i != end; i += step) {
        spr_px.color = px_buf[i];
        spr_px.x = spr.x() + (reversed ? (px_buf.size()-1-i) : i);
        spr_px.palette = spr.palette();
        spr_px.priority = spr.priority();
        
        if(!px_occupied(fifo, spr_px) && spr_px.x >= SCREEN_X_OFFSET) {
            fifo.push(spr_px);
        }
    }

    if(spr_queue.empty()) {
        stop();
    } else {
        reset_fetch();
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