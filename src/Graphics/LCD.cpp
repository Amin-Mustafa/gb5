#include "../../include/Graphics/LCD.h" 
#include <SDL2/SDL.h>
#include <string>

class SDL_error{
public:
	SDL_error(std::string message) :msg(message) {}
	~SDL_error() {}
	
	const char* what() const { return msg.c_str(); }
	
private:
	std::string& msg;
};


LCD::LCD(unsigned int window_scale)
    :scale{window_scale}
     {
        init();
        //std::fill(buffer.begin(), buffer.end(), Color::WHITE);
     }

void LCD::init() {
    if(SDL_Init(SDL_INIT_VIDEO) < 0) {
        throw SDL_error("SDL Init error: " + std::string{SDL_GetError()});
    }

    //init window
    window = WindowPtr (
        SDL_CreateWindow (
            "gb5",
            SDL_WINDOWPOS_UNDEFINED,
            SDL_WINDOWPOS_UNDEFINED,
            SCREEN_WIDTH * scale,
            SCREEN_HEIGHT * scale, 
            SDL_WINDOW_SHOWN
        ), 
        SDL_DestroyWindow
    );
    if(!window) {
        throw SDL_error("SDL Window create error: " + std::string{SDL_GetError()});
    }

    //init renderer
    renderer = RendererPtr (
        SDL_CreateRenderer(
            window.get(), 
            -1,
            SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC
        ),
        SDL_DestroyRenderer
    );
    if(!renderer) {
        throw SDL_error("SDL Renderer create error: " + std::string{SDL_GetError()});
    }

    SDL_SetRenderDrawColor(renderer.get(), 255, 255, 255, 255);
    SDL_RenderClear(renderer.get());	

    //init texture
    texture = TexturePtr (
        SDL_CreateTexture (
            renderer.get(),
            SDL_PIXELFORMAT_ARGB8888,
            SDL_TEXTUREACCESS_STREAMING,
            SCREEN_WIDTH,
            SCREEN_HEIGHT
        ),
        SDL_DestroyTexture
    );
}

void LCD::draw_frame() {
    SDL_UpdateTexture(
        texture.get(),
        nullptr,
        buffer.data(),
        SCREEN_WIDTH * sizeof(PixelFormat)
    );
    SDL_RenderCopy(renderer.get(), texture.get(), nullptr, nullptr);
	SDL_RenderPresent(renderer.get());	
}