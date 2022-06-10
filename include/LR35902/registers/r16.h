#pragma once

#include "../config.h"
#include "LR35902/stubs/bus/bus.h"

#include <cstdint>

namespace LR35902 {
class r8;

class r16 {
private:
  Bus &m_bus;

  r8 &m_hi;
  r8 &m_lo;

public:
  r16() = delete;
  r16(Bus &bus, r8 &hi, r8 &lo) : m_bus{bus}, m_hi{hi}, m_lo{lo} {
  }

  [[nodiscard]] std::uint16_t data() const noexcept;
  byte &operator*(const r16 rr) noexcept;
};

} // namespace LR35902
