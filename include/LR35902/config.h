#pragma once

#include <cstddef>
#include <cstdint>

namespace LR35902 {

using flag = bool;
using byte = std::uint8_t;
using word = std::uint16_t;

[[nodiscard]] consteval unsigned long long operator"" _B(const unsigned long long l) {
  return l;
}

[[nodiscard]] consteval unsigned long long operator"" _KiB(const unsigned long long l) {
  return 1024 * l;
}

[[nodiscard]] consteval unsigned long long operator"" _MiB(const unsigned long long l) {
  return 1024_KiB * l;
}

} // namespace LR35902
