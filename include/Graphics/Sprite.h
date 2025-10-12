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
public:
    uint8_t color = 0;
    uint8_t x = 0;
    Sprite::Palette palette = Sprite::Palette::OBP0;
    Sprite::Priority priority = Sprite::Priority::BACK;
};

#endif