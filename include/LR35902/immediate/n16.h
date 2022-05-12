#pragma once

#include <cstdint>

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

} // namespace LR35902
