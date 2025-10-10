#ifndef SPRITE_H
#define SPRITE_H

#include <cstdint>
#include <span>
#include <stdexcept>

class Sprite {
private:
    std::span<const uint8_t, 4> data;
    static constexpr uint8_t init[4] = {0};
public:
    Sprite()
        :data{init, 4} {}
    Sprite(const uint8_t* bytes) 
        :data{bytes, 4} {}

    enum class Palette : uint8_t {OBP0, OBP1};
    enum class Priority : uint8_t {FRONT, BACK};

    //information
    uint8_t y() const {return data[0];}
    uint8_t x() const {return data[1];}
    uint8_t index() const {return data[2];}

    //attributes
    bool y_flip() const {return (data[3] >> 6) & (uint8_t)1;}
    bool x_flip() const {return (data[3] >> 5) & (uint8_t)1;}
    Priority priority() const {
        return static_cast<Priority>((data[3] >> 7) & (uint8_t)1);
    }
    Palette palette() const {
        return static_cast<Palette>((data[3] >> 4) & (uint8_t)1);
    }
};  

class SpriteBuffer {
private:   
    std::array<Sprite, 10> buf;
    uint8_t sprite_count = 0;
public:
    const Sprite& at(uint8_t index) const {
        if(index >= sprite_count) {
            throw std::out_of_range("Sprite index out of range");
        }
        return buf[index];
    }
    uint8_t count() const {return sprite_count;}

    void push_sprite(const Sprite& spr) {
        if(sprite_count == buf.size()) {
            return;
        }
        buf[sprite_count] = spr;
        sprite_count++;
    }
    void clear() {
        sprite_count = 0;
    }
}; 

class SpritePixel {
private:
    uint8_t col;
    uint8_t x_pos;
    uint8_t pal;
    uint8_t prio;
public:
    SpritePixel()
        :col{0}, x_pos{0}, pal{0}, prio{0} {}

    SpritePixel(uint8_t color, uint8_t x, Sprite::Palette palette, Sprite::Priority priority)
        :col{color},
         x_pos{x},
         pal{static_cast<uint8_t>(palette)}, 
         prio{static_cast<uint8_t>(priority)} {}

    uint8_t color() const {
        return col;
    }
    //get visible x position
    uint8_t x() const {
        return x_pos - 8;
    }
    Sprite::Palette palette() const {
        return static_cast<Sprite::Palette>(pal);
    }
    Sprite::Priority priority() const {
        return static_cast<Sprite::Priority>(prio);
    }
};

#endif