#include "SDL2/SDL.h"

class JoyPad;

class InputHandler {
private:
    const Uint8* key_state;

public:
    //button mappings
    enum class Mapping : Uint8 {
        LEFT   = SDL_SCANCODE_A,
        RIGHT  = SDL_SCANCODE_D,
        DOWN   = SDL_SCANCODE_S,
        UP     = SDL_SCANCODE_W,
        A      = SDL_SCANCODE_L,
        B      = SDL_SCANCODE_K,
        START  = SDL_SCANCODE_RETURN,
        SELECT = SDL_SCANCODE_SPACE,
    };

    void get_key_state() {
        key_state = SDL_GetKeyboardState(NULL);
    }
    bool key_pressed(Mapping key) {
        return key_state[static_cast<SDL_Scancode>(key)];
    }
};  