#pragma once

#include "../config.h"
#include "../stubs/bus/bus.h"

#include <cstdint>

namespace LR35902 {

struct n16 {
  std::uint16_t m_data{};

  n16 &operator++(int) noexcept;
  const n16 operator++() noexcept;

  n16 &operator--(int) noexcept;
  const n16 operator--() noexcept;
};

byte &operator*(const n16 nn) noexcept;

} // namespace LR35902
