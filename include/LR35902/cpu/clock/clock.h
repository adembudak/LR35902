#pragma once

#include <cstddef>

namespace LR35902 {

class clock {
  std::size_t m_data{};
  std::size_t m_latest{};

public:
  void cycle(const std::size_t t) {
    m_latest = t;

    for(std::size_t i = 0; i < t; ++i)
      ++m_data;
  }

  [[nodiscard]] auto data() const noexcept -> std::size_t {
    return m_data;
  }

  [[nodiscard]] auto latest() const noexcept -> std::size_t {
    return m_latest;
  }

  friend class DebugView;
};

}
