#pragma once

#include <cstdint>

namespace LR35902 {

struct e8 {
  std::int8_t m_data{};

  e8 &operator++(int) noexcept;
  const e8 operator++() noexcept;

  e8 &operator--(int) noexcept;
  const e8 operator--() noexcept;
};

} // namespace LR35902
