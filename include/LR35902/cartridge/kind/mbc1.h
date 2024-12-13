#pragma once

#include <LR35902/config.h>

#include <cstddef>
#include <vector>

namespace LR35902 {

class mbc1 final {
  std::vector<byte> m_rom;
  std::vector<byte> m_sram;
  bool has_sram;
  bool has_battery;

  byte register_0 = 0;
  byte register_1 = 0;
  byte register_2 = 0;
  byte register_3 = 0;

public:
  explicit mbc1(std::vector<byte> other, const std::size_t RAM_size, const bool has_battery);

  [[nodiscard]] byte readROM(const address_t index) const noexcept;
  void writeROM(const address_t index, const byte b) noexcept;

  [[nodiscard]] byte readSRAM(address_t index) const noexcept;
  void writeSRAM(address_t index, const byte b) noexcept;

  friend class Cartridge;
};

}
