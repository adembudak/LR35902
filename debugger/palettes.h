#pragma once

#include <array>
#include <cstdint>

#include <SDL2/SDL_pixels.h>

using palette_t = std::array<SDL_Colour, 4>;

const palette_t original = {
    SDL_Colour{107, 166, 74},
    SDL_Colour{67,  122, 99},
    SDL_Colour{37,  89,  85},
    SDL_Colour{18,  66,  76}
};

const palette_t cococola = {
    SDL_Colour{244, 0, 9},
    SDL_Colour{186, 0, 6},
    SDL_Colour{114, 0, 4},
    SDL_Colour{43,  0, 1}
};

const palette_t galata = {
    SDL_Colour{117, 148, 166},
    SDL_Colour{101, 130, 144},
    SDL_Colour{32,  46,  72 },
    SDL_Colour{34,  63,  69 }
};
