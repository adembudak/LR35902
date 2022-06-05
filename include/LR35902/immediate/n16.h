#pragma once

#include "../config.h"
#include "../stubs/bus/bus.h"

#include <cstdint>

namespace LR35902 {

struct n16 {
  std::uint16_t m_data{};
};

byte &operator*(const n16 nn) noexcept;

} // namespace LR35902
