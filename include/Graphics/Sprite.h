#ifndef SPRITE_H
#define SPRITE_H

#include <cstdint>
#include <span>

class Sprite {
private:
    std::span<const uint8_t, 4> data;
public:
    Sprite(const uint8_t* bytes) 
        :data{bytes, 4} {}

    //information
    uint8_t y() const {return data[0];}
    uint8_t x() const {return data[1];}
    uint8_t index() const {return data[2];}

    //attributes
    bool priority() const {return (data[3] >> 7) & (uint8_t)1;}
    bool y_flip() const {return (data[3] >> 6) & (uint8_t)1;}
    bool x_flip() const {return (data[3] >> 5) & (uint8_t)1;}
    bool palette() const {return (data[3] >> 4) & (uint8_t)1;}
};  

#endif