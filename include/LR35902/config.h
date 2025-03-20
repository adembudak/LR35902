#pragma once

#include <cstddef>
#include <cstdint>
#include <random>

namespace LR35902 {

using flag = bool;
using byte = std::uint8_t;
using sbyte = std::int8_t;
using word = std::uint16_t;
using address_t = std::uint16_t;

[[nodiscard]] consteval unsigned long long operator""_B(const unsigned long long l) noexcept {
  return l;
}

[[nodiscard]] consteval unsigned long long operator""_KiB(const unsigned long long l) noexcept {
  return 1024_B * l;
}

[[nodiscard]] consteval unsigned long long operator""_MiB(const unsigned long long l) noexcept {
  return 1024_KiB * l;
}

[[nodiscard]] consteval unsigned long long operator""_ROMBANK(const unsigned long long l) noexcept {
  return 16_KiB * l;
}

[[nodiscard]] consteval unsigned long long operator""_SRAMBANK(const unsigned long long l) noexcept {
  return 8_KiB * l;
}

inline std::mt19937 engine{std::random_device{}()};
inline std::uniform_int_distribution get{0, 255};
[[nodiscard]] inline byte random_byte() {
  return get(engine);
}

} // namespace LR35902
