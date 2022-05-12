#pragma once

#include <cstdint>

namespace LR35902 {

struct e8 {
  std::int8_t m_data{};

  e8 &operator++(int) noexcept {
    ++m_data;
    return *this;
  }

  const e8 operator++() noexcept {
    const auto temp = *this;
    ++m_data;
    return temp;
  }

  e8 &operator--(int) noexcept {
    --m_data;
    return *this;
  }

  const e8 operator--() noexcept {
    const auto temp = *this;
    --m_data;
    return temp;
  }
};

} // namespace LR35902
