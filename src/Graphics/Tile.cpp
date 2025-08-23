#include "../../include/Arithmetic.h"
#include "../../include/Graphics/Tile.h"

Tile::Tile(const uint8_t* data)
    :tile_data{data, 16} {}

uint8_t Tile::get_pixel(uint8_t x, uint8_t y) const {
    //0 <= x,y <= 7
    //return color index of pixel at (x,y) in tile 
    uint8_t pixel_lsb = Arithmetic::bit_check(tile_data[y], x);
    uint8_t pixel_msb = Arithmetic::bit_check(tile_data[y + 1], x);
    return (pixel_msb << 1) | pixel_lsb;   
}