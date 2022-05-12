#pragma once

#include "../config.h"

namespace LR35902 {

// https://gbdev.io/pandocs/CPU_Registers_and_Flags.html#the-flags-register-lower-8-bits-of-af-register

struct flags {
  bool z = false;
  bool n = false;
  bool h = false;
  bool c = false;

  byte data() const noexcept { // 0bZNHC'0000
    return byte(z << 7 | n << 6 | h << 5 | c << 4);
  }
};

} // namespace LR35902
