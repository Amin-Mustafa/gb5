#include "SDL2/SDL.h"

class JoyPad;

class InputHandler {
private:
    const Uint8* key_state;

public:
    //button mappings
    enum class Mapping : int {
        LEFT   = SDL_SCANCODE_LEFT,
        RIGHT  = SDL_SCANCODE_RIGHT,
        DOWN   = SDL_SCANCODE_DOWN,
        UP     = SDL_SCANCODE_UP,
        A      = SDL_SCANCODE_Z,
        B      = SDL_SCANCODE_X,
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