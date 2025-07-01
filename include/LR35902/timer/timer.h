#pragma once

#include <cstddef>
#include <cstdint>

namespace LR35902 {

class IO;
class Interrupt;

class Timer {
  IO &m_io;
  Interrupt &m_intr;

  std::uint16_t counter = 0;
  std::uint16_t previous_counter = 0;
  std::uint16_t div_counter = 0;

public:
  Timer(IO &io, Interrupt &intr);
  void update(const std::size_t cycles) noexcept;
};

}
