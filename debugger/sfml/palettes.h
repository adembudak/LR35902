#pragma once

#include <array>
#include <cstdint>

#include <SFML/Graphics/Color.hpp>

using palette_t = std::array<sf::Color, 4>;

const palette_t original = {
    sf::Color{107, 166, 74},
    sf::Color{67,  122, 99},
    sf::Color{37,  89,  85},
    sf::Color{18,  66,  76}
};

const palette_t cococola = {
    sf::Color{244, 0, 9},
    sf::Color{186, 0, 6},
    sf::Color{114, 0, 4},
    sf::Color{43,  0, 1}
};

const palette_t galata = {
    sf::Color{117, 148, 166},
    sf::Color{101, 130, 144},
    sf::Color{32,  46,  72 },
    sf::Color{34,  63,  69 }
};
