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

  r8 &operator<<=(const std::uint8_t u) noexcept;
  r8 &operator>>=(const std::uint8_t u) noexcept;

  auto operator<=>(const r8 &) const = default;
  bool operator==(const r8 &) const = default;

  auto operator<=>(const byte b) const {
    return m_data <=> b;
  }

  bool operator==(const byte b) const {
    return m_data == b;
  }

  auto operator<=>(const n8 n) const {
    return m_data <=> n.m_data;
  }

  bool operator==(const n8 n) const {
    return m_data == n.m_data;
  }

  static constexpr std::uint8_t min() noexcept {
    return 0b0000'0000;
  }

  static constexpr std::uint8_t max() noexcept {
    return 0b1111'1111;
  }
};

byte operator+(const r8 l, const r8 r) noexcept;
byte operator+(const r8 r, const byte b) noexcept;
byte operator+(const r8 r, const n8 n) noexcept;

byte operator-(const r8 l, const r8 r) noexcept;
byte operator-(const r8 r, const byte b) noexcept;
byte operator-(const r8 r, const n8 n) noexcept;

} // namespace LR35902
