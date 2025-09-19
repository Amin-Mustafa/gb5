#ifndef LCD_H
#define LCD_H

#include <cstdint>
#include <array>

class LCD {
private:
    static constexpr int SCREEN_HEIGHT = 144;
    static constexpr int SCREEN_WIDTH  = 160;
public:
    std::array<std::array<uint8_t, SCREEN_WIDTH>, SCREEN_HEIGHT> buffer;
    void blit(uint8_t px, uint8_t x, uint8_t y) {
        buffer[x][y] = px;
    } 
    constexpr int width() const {return SCREEN_WIDTH;}
    constexpr int height() const {return SCREEN_HEIGHT;}
};

#endif