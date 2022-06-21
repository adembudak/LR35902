#pragma once

#include <LR35902/config.h>
#include <LR35902/stubs/bus/bus.h>

#include <cstdint>

namespace LR35902 {

struct n16 {
  std::uint16_t m_data{};

  inline std::uint16_t operator++(int) noexcept {
    auto temp = m_data;
    ++m_data;
    return temp;
  }
};

} // namespace LR35902