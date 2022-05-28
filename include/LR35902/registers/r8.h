#pragma once

#include "../config.h"
#include "../immediate/n8.h"

#include <vector>

namespace LR35902 {

class r8 {
private:
  byte m_data = 0;

public:
  r8 &operator=(const byte b) noexcept {
    m_data = b;
    return *this;
  }

  r8 &operator=(const n8 n) noexcept {
    m_data = n.m_data;
    return *this;
  }

  /*
  byte operator*(const r8 r) {
    return m_bus[r.data()];
  }
  */

  uint8_t data() const noexcept {
    return m_data;
  }

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
};

static_assert(sizeof(r8) == sizeof(byte));
} // namespace LR35902
