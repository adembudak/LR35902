#pragma once

#include <cstdint>

namespace LR35902 {

class clock {
  std::size_t m_data{};

public:
  void cycle(const std::size_t t) {
    for(std::size_t i = 0; i < t; ++i)
      ++m_data;
  }

  [[nodiscard]] auto data() const noexcept -> std::size_t {
    return m_data;
  }

  friend class DebugView;
};

}
