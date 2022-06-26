#pragma once

#include <LR35902/config.h>
#include <LR35902/core/immediate/n8.h>

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

  r8 &operator&=(const r8 r) noexcept;
  r8 &operator&=(const byte b) noexcept;
  r8 &operator&=(const n8 n) noexcept;

  r8 &operator|=(const r8 r) noexcept;
  r8 &operator|=(const byte b) noexcept;
  r8 &operator|=(const n8 n) noexcept;

  r8 &operator^=(const r8 r) noexcept;
  r8 &operator^=(const byte b) noexcept;
  r8 &operator^=(const n8 n) noexcept;

  auto operator<=>(const r8 &) const = default;
  bool operator==(const r8 &) const = default;

  auto operator<=>(const std::uint8_t &u) const {
    return m_data <=> u;
  }

  bool operator==(const std::uint8_t &u) const {
    return m_data == u;
  }

  static constexpr std::uint8_t min() noexcept {
    return 0b0000'0000;
  }

  static constexpr std::uint8_t max() noexcept {
    return 0b1111'1111;
  }
};

} // namespace LR35902
