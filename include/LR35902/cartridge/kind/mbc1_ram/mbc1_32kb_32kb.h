#pragma once

#include <LR35902/config.h>

#include <array>
#include <cstddef>
#include <vector>

namespace LR35902 {

class mbc1_32kb_32kb final {
  std::vector<byte> m_rom;
  std::array<byte, 32_KiB> m_sram;

  int register_0 = 0;
  int register_1 = 0;
  int register_2 = 0;
  int register_3 = 0;

public:
  explicit mbc1_32kb_32kb(std::vector<byte> other);

  [[nodiscard]] byte readROM(const std::size_t index) const noexcept;
  void writeROM(const std::size_t index, const byte b) noexcept;

  [[nodiscard]] byte readSRAM(const std::size_t index) const noexcept;
  void writeSRAM(const std::size_t index, const byte b) noexcept;

  friend class Cartridge;
};

}
