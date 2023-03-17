#pragma once

#include <LR35902/config.h>

#include <vector>

namespace LR35902 {

class bootROM {
  std::vector<byte> m_data{};

public:
  void load() noexcept;
  byte read(const std::size_t index) const noexcept;
  bool isBootOnGoing() const noexcept;
  void unmap() noexcept;
};

}
