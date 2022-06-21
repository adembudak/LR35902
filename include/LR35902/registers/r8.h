#pragma once

#include "../config.h"
#include "../immediate/n8.h"

#include <compare>
#include <cstdint>

namespace LR35902 {

class r8 {
private:
  byte m_data = 0;

public:
  r8 &operator=(const byte b) noexcept;
  r8 &operator=(const n8 n) noexcept;

  [[nodiscard]] std::uint8_t lowNibble() const noexcept;
  [[nodiscard]] std::uint8_t highNibble() const noexcept;

  [[nodiscard]] std::uint8_t data() const noexcept;

  r8 &operator++() noexcept;
  r8 operator++(int) noexcept;

  r8 &operator--() noexcept;
  r8 operator--(int) noexcept;

  r8 &operator+=(const r8 r) noexcept;
  r8 &operator-=(const r8 r) noexcept;

  r8 &operator+=(const n8 n) noexcept;
  r8 &operator-=(const n8 n) noexcept;

  r8 &operator+=(const byte r) noexcept;
  r8 &operator-=(const byte r) noexcept;

  auto operator<=>(const r8 &) const = default;

  static constexpr std::uint8_t min() noexcept {
    return 0b0000'0000;
  }

  static constexpr std::uint8_t max() noexcept {
    return 0b1111'1111;
  }
};

} // namespace LR35902
