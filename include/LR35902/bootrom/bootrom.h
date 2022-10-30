#pragma once

#include <LR35902/config.h>

#include <vector>

namespace LR35902 {

class bootROM {
  std::vector<byte> m_data{};

public:
  void load() noexcept;

  void unmap() noexcept;
  bool isBootOnGoing() const noexcept;
  byte &read(std::size_t index) noexcept;
};

}
