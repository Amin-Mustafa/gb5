#ifndef LCD_H
#define LCD_H

#include <cstdint>
#include <array>
#include <memory>
#include <SDL2/SDL.h>  

class LCD {
private:
    using PixelFormat = uint32_t;

    static constexpr int SCREEN_HEIGHT = 144;
    static constexpr int SCREEN_WIDTH  = 160;

    enum Color : uint32_t {
        WHITE = 0xFFFFFFFF,
        LIGHT_GRAY = 0xFFA9A9A9,
        DARK_GRAY = 0xFF545454,
        BLACK = 0xFF000000
    };

    std::array<PixelFormat, SCREEN_WIDTH * SCREEN_HEIGHT> buffer;
    unsigned int scale;
    uint32_t color_palette[4] = {
        WHITE, LIGHT_GRAY, DARK_GRAY, BLACK
    };

public:
    LCD(unsigned int window_scale);

    void init();
    void blit(uint8_t px, uint8_t x, uint8_t y) {
        buffer[y * SCREEN_WIDTH + x] = color_palette[px];
    } 
    void draw_frame();
    constexpr int width() const {return SCREEN_WIDTH;}
    constexpr int height() const {return SCREEN_HEIGHT;}

private:
    using WindowPtr   = std::unique_ptr<SDL_Window, decltype(&SDL_DestroyWindow)>;
    using RendererPtr = std::unique_ptr<SDL_Renderer, decltype(&SDL_DestroyRenderer)>;
    using TexturePtr  = std::unique_ptr<SDL_Texture, decltype(&SDL_DestroyTexture)>;
    
    WindowPtr window{nullptr, SDL_DestroyWindow};
    RendererPtr renderer{nullptr, SDL_DestroyRenderer};
    TexturePtr texture{nullptr, SDL_DestroyTexture};
};

#endif