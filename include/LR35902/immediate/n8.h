#pragma once

#include <cstdint>

namespace LR35902 {

struct n8 {
  std::uint8_t m_data{};

  n8 &operator++(int) noexcept;
  const n8 operator++() noexcept;

  n8 &operator--(int) noexcept;
  const n8 operator--() noexcept;
};

} // namespace LR35902
