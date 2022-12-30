#pragma once

#include <LR35902/config.h>

namespace LR35902 {

class IO;
class Interrupt;

// clang-format off
enum class button : std::uint8_t {
  up, right, down, left, // direction keys
  a, b, select, start    // selection keys
};

enum class keystatus : std::uint8_t { 
   pressed = 0, released = 1
};

class Joypad {
  byte m_state{};
  IO &m_io;
  Interrupt &m_intr;

public:
  Joypad(IO &io, Interrupt &intr) noexcept;

  void update(button btn, keystatus status) noexcept;
  [[nodiscard]] byte read() const noexcept;
  [[nodiscard]] bool isBlocked() const noexcept;
};


}
