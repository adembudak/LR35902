#pragma once

#include "../config.h"

namespace LR35902 {

class r8 {
private:
  byte m_data = 0;

public:
  uint8_t data() const noexcept { return m_data; }

  r8 &operator++(int) noexcept {
    ++m_data;
    return *this;
  }

  const r8 operator++() noexcept {
    const auto temp = *this;
    ++m_data;
    return temp;
  }

  r8 &operator--(int) noexcept {
    --m_data;
    return *this;
  }

  const r8 operator--() noexcept {
    const auto temp = *this;
    --m_data;
    return temp;
  }

  //
};

} // namespace LR35902
