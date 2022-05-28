#pragma once

#include "../config.h"
#include <cstdint>
#include <vector>

namespace LR35902 {

struct n16 {
  std::uint16_t m_data{};

  n16 &operator++(int) noexcept {
    ++m_data;
    return *this;
  }

  const n16 operator++() noexcept {
    const auto temp = *this;
    ++m_data;
    return temp;
  }

  n16 &operator--(int) noexcept {
    --m_data;
    return *this;
  }

  const n16 operator--() noexcept {
    const auto temp = *this;
    --m_data;
    return temp;
  }
};

std::vector<byte> bb(1000);

byte &operator*(const n16 nn) {
  // return readbus(rr.value());
  return bb[nn.m_data];
}

} // namespace LR35902
