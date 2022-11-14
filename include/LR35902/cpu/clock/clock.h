#pragma once

#include <cstddef>

namespace LR35902 {

class Clock {
  std::size_t m_data{};
  std::size_t m_latest{};

public:
  void cycle(const std::size_t m) {
    m_latest = m;
    m_data += m;
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
