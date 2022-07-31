#pragma once

#include <LR35902/bus/bus.h>
#include <LR35902/config.h>

#include <cstdint>

namespace LR35902 {

struct n16 {
  std::uint16_t m_data{};

  void lo(const std::uint8_t n) noexcept {
    m_data &= 0b1111'1111'0000'0000; // reset lower byte
    m_data |= n;                     // set it with n
  }

  void hi(const std::uint8_t n) noexcept {
    m_data &= 0b0000'0000'1111'1111; // reset upper byte
    m_data |= std::uint16_t(n << 8); // set upper byte by n
  }

  std::uint8_t lo() const noexcept {
    return std::uint8_t(m_data & 0b0000'0000'1111'1111);
  }

  std::uint8_t hi() const noexcept {
    return std::uint8_t((m_data & 0b1111'1111'0000'0000) >> 8);
  }

  std::uint16_t operator++(int) noexcept {
    auto temp = m_data;
    ++m_data;
    return temp;
  }

  n16 &operator--() noexcept {
    --m_data;
    return *this;
  }
};

} // namespace LR35902
