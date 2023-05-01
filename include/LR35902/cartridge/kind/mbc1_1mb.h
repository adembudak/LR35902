#pragma once

#include <LR35902/config.h>

#include <cstddef>
#include <vector>

namespace LR35902 {

class mbc1_1mb final {
  std::vector<byte> m_rom;

  int register_0 = 0;
  int register_1 = 0;
  int register_2 = 0;
  int register_3 = 0;

public:
  explicit mbc1_1mb(std::vector<byte> other);

  [[nodiscard]] byte readROM(const std::size_t index) const noexcept;
  void writeROM(const std::size_t index, const byte b) noexcept;

  friend class Cartridge;
};

}
