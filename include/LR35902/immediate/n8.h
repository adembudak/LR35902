#pragma once

#include <cstdint>

namespace LR35902 {

struct n8 {
  std::uint8_t m_data{};

  n8 &operator++(int) noexcept {
    ++m_data;
    return *this;
  }

  const n8 operator++() noexcept {
    const auto temp = *this;
    ++m_data;
    return temp;
  }

  n8 &operator--(int) noexcept {
    --m_data;
    return *this;
  }

  const n8 operator--() noexcept {
    const auto temp = *this;
    --m_data;
    return temp;
  }
};

} // namespace LR35902
