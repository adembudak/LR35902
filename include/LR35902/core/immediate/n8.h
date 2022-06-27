#pragma once

#include <cstdint>

namespace LR35902 {

struct n8 {
  std::uint8_t m_data{};

  [[nodiscard]] std::uint8_t lowNibble() const noexcept {
    return m_data & 0b0000'1111;
  }

  [[nodiscard]] std::uint8_t highNibble() const noexcept {
    return (m_data & 0b1111'0000) >> 4;
  }
};

} // namespace LR35902
