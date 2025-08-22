#ifndef TILE_H
#define TILE_H

#include <cstdint>
#include <array>

class MMU;

class Tile {
//a tile is an 8x8 pixel chunk of graphics
//a pixel is a 2-bit color index (0 to 3)
//hence each tile is 16 bytes (128 bits = 64px)
private:
    //data stored in a 16-byte array
    //the first 2 bytes are the "front" and "back" of the first line
    //the second 2 bytes are those of the second line, etc.
    std::array<uint8_t, 16> tile_data; 
    uint16_t start_addr;
public:
    Tile(uint16_t address=0);
    uint8_t get_pixel(uint8_t x, uint8_t y);

    uint8_t read(uint16_t addr) {
        return tile_data[addr - start_addr];
    }
    void write(uint16_t addr, uint8_t val) {
        tile_data[addr - start_addr] = val;
    }
};

class TileBlock {
private:
    //just a container of 128 tiles
    uint16_t start_addr;
    std::array<Tile, 128> tiles;
public:

    TileBlock(uint16_t addr);
    
    uint16_t start() const {return start_addr;}
    Tile& tile_at(uint16_t addr) {
        return tiles[(addr-start_addr)/0x10];
    }
    uint8_t read(uint16_t addr) {
        return tile_at(addr).read(addr);
    }
    void write(uint16_t addr, uint8_t val) {
        tile_at(addr).write(addr, val);
    }
};

#endif