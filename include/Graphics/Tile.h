#ifndef TILE_H
#define TILE_H

#include <cstdint>
#include <array>
#include <span>

class MMU;

class Tile {
    //read only tile view of 16-byte memory array
    //a tile is an 8x8 pixel chunk of pixels
    //a pixel is a 2-bit color index (0 to 3)
    //hence each tile is 16 bytes (128 bits = 64px)
private:
    //the first 2 bytes are the "front" and "back" of the first line
    //the second 2 bytes are those of the second line, etc.
    std::span<const uint8_t, 16> tile_data;
public:
    Tile(const uint8_t* data);
    uint8_t get_pixel(uint8_t x, uint8_t y) const;
};

#endif