#include "../../include/Graphics/Tile.h"
#include "../../include/Arithmetic.h"
#include <algorithm>

Tile::Tile(uint16_t address)
    :start_addr{address}, tile_data{} {}

uint8_t Tile::get_pixel(uint8_t x, uint8_t y) {
    //0 <= x,y <= 7
    //return color index of pixel at (x,y) in tile 
    uint8_t pixel_lsb = Arithmetic::bit_check(tile_data[y], x);
    uint8_t pixel_msb = Arithmetic::bit_check(tile_data[y + 1], x);
    return (pixel_msb << 1) | pixel_lsb;   
}

TileBlock::TileBlock(uint16_t addr) 
    :start_addr{addr}
    {
        for (size_t i = 0; i < tiles.size(); ++i)
            tiles[i] = Tile(start_addr + i * 0x0f);
    }