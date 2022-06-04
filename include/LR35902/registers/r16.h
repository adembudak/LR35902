#pragma once

#include "../stubs/bus/bus.h"
#include "flags.h"
#include "r8.h"

namespace LR35902 {

class r16 {
private:
  r8 &m_hi;
  r8 &m_lo;

public:
  r16(r8 &hi, r8 &lo) : m_hi{hi}, m_lo{lo} {
  }

  std::uint16_t data() const noexcept {
    return std::uint16_t(m_hi.data() << 8 | m_lo.data());
  }
};

static_assert(sizeof(r16) == (sizeof(byte *) + sizeof(byte *)));

byte &operator*(const r16 rr) noexcept {
  return Bus::reach()[rr.data()];
}

} // namespace LR35902
