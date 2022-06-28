#pragma once

#include <LR35902/config.h>

#include <compare>
#include <cstdint>

namespace LR35902 {
class r8;
class n16;
class Bus;

class r16 {
private:
  Bus &m_bus;

  r8 &m_hi;
  r8 &m_lo;

public:
  r16() = delete;
  r16(Bus &bus, r8 &hi, r8 &lo) :
      m_bus{bus},
      m_hi{hi},
      m_lo{lo} {}

  r16 &operator=(const n16 nn) noexcept;

  [[nodiscard]] r8 lo() const noexcept;
  [[nodiscard]] r8 hi() const noexcept;

  [[nodiscard]] std::uint16_t data() const noexcept;
  byte &operator*() noexcept;

  r16 &operator++() noexcept;
  r16 &operator--() noexcept;

  r16 &operator+=(const r16 rr) noexcept;
  r16 &operator-=(const r16 rr) noexcept;

  r16 &operator+=(const n16 nn) noexcept;
  r16 &operator-=(const n16 nn) noexcept;

  auto operator<=>(const r16 rr) const noexcept {
    return data() <=> rr.data();
  }

  bool operator==(const r16 rr) const noexcept {
    return data() == rr.data();
  }

  static constexpr std::uint16_t min() noexcept {
    return 0b0000'0000'0000'0000;
  }

  static constexpr std::uint16_t max() noexcept {
    return 0b1111'1111'1111'1111;
  }
};

} // namespace LR35902
