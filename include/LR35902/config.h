#pragma once

#include <cstddef>
#include <cstdint>

namespace LR35902 {

using byte = std::uint8_t;

inline unsigned long long operator"" _B(const unsigned long long l) { //
  return l;
}

inline unsigned long long operator"" _KiB(const unsigned long long l) {
  return 1024 * l;
}

inline unsigned long long operator"" _MiB(const unsigned long long l) {
  return 1024_KiB;
}

} // namespace LR35902
