#pragma once

#include "../config.h"

namespace LR35902 {

// 0bZNHC'0000
// https://gbdev.io/pandocs/CPU_Registers_and_Flags.html#the-flags-register-lower-8-bits-of-af-register

class flags {
public:
  bool z = false;
  bool n = false;
  bool h = false;
  bool c = false;

  byte data() const noexcept {
    // let's say z, n, h c are all set, to return them as a byte:
    static_assert((1 << 7 | 1 << 6 | 1 << 5 | 1 << 4) == 0b1111'0000);

    return (z << 7 | n << 6 | h << 5 | c << 4);
  }
};

} // namespace LR35902
