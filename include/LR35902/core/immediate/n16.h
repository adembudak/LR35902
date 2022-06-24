#pragma once

#include <LR35902/config.h>
#include <LR35902/stubs/bus/bus.h>

#include <cstdint>

namespace LR35902 {

struct n16 {
  std::uint16_t m_data{};

  inline std::uint8_t lo() const noexcept {
    return std::uint8_t(m_data & 0b0000'0000'1111'1111);
  }

  inline std::uint8_t hi() const noexcept {
    return std::uint8_t((m_data & 0b1111'1111'0000'0000) >> 8);
  }

  inline std::uint16_t operator++(int) noexcept {
    auto temp = m_data;
    ++m_data;
    return temp;
  }
};

} // namespace LR35902
