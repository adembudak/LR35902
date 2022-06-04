#pragma once

#include "../stubs/bus/bus.h"

#include <cstdint>

namespace LR35902 {
class r8;

class r16 {
private:
  r8 &m_hi;
  r8 &m_lo;

public:
  r16() = delete;
  r16(r8 &hi, r8 &lo);

  std::uint16_t data() const noexcept;
};

byte &operator*(const r16 rr) noexcept;

} // namespace LR35902
