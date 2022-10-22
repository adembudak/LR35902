#pragma once

#include <array>
#include <cstdint>

namespace LR35902 {

struct rgba32 {
  std::uint8_t r, g, b, a = 255;
};

using palette_t = std::array<rgba32, 4>;

constexpr palette_t original = {
    rgba32{107, 166, 74},
    rgba32{67,  122, 99},
    rgba32{37,  89,  85},
    rgba32{18,  66,  76}
};

constexpr palette_t cococola = {
    rgba32{244, 0, 9},
    rgba32{186, 0, 6},
    rgba32{114, 0, 4},
    rgba32{43,  0, 1}
};

constexpr palette_t galata = {
    rgba32{117, 148, 166},
    rgba32{101, 130, 144},
    rgba32{32,  46,  72 },
    rgba32{34,  63,  69 }
};

}
