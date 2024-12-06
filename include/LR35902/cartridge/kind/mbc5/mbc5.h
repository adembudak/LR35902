#pragma once

#include <LR35902/config.h>

#include <cstddef>
#include <vector>

namespace LR35902 {

class mbc5 final {
  std::vector<byte> m_rom;
  std::vector<byte> m_sram;

  byte ramg;
  byte romb_0;
  byte romb_1;
  byte ramb;

  bool has_battery;
  bool has_rumble;

public:
  mbc5(std::vector<byte> other, std::size_t RAM_size, bool has_battery, bool has_rumble);

  [[nodiscard]] byte readROM(const std::size_t index) const noexcept;
  void writeROM(const std::size_t index, const byte b) noexcept;

  [[nodiscard]] byte readSRAM(std::size_t index) const noexcept;
  void writeSRAM(std::size_t index, const byte b) noexcept;


  friend class Cartridge;
};

}
