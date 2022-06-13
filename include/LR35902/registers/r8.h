#pragma once

#include "../config.h"
#include "../immediate/n8.h"

#include <cstdint>

namespace LR35902 {

class r8 {
private:
  byte m_data = 0;

public:
  r8 &operator=(const byte b) noexcept;
  r8 &operator=(const n8 n) noexcept;

  [[nodiscard]] std::uint8_t data() const noexcept;

  r8 &operator++() noexcept;
  r8 operator++(int) noexcept;

  r8 &operator--() noexcept;
  r8 operator--(int) noexcept;
};

static_assert(sizeof(r8) == sizeof(byte));
} // namespace LR35902
