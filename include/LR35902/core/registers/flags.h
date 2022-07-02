#pragma once

#include <LR35902/config.h>

namespace LR35902 {

// https://gbdev.io/pandocs/CPU_Registers_and_Flags.html#the-flags-register-lower-8-bits-of-af-register

////////////////////////////
///  z: zero flag        ///
///  n: negation flag    ///
///  h: half carry flag  ///
///  c: carry flag       ///
///  0bznhc'0000         ///
////////////////////////////

struct flags {
  bool z = false;
  bool n = false;
  bool h = false;
  bool c = false;

  [[nodiscard]] byte data() const noexcept {
    return byte(z << 7 | n << 6 | h << 5 | c << 4);
  }
};

} // namespace LR35902
