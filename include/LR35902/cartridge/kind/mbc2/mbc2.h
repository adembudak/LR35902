#pragma once

#include <LR35902/config.h>

#include <array>
#include <cstddef>
#include <vector>

namespace LR35902 {

class mbc2 final {
  std::vector<byte> m_rom;
  std::array<byte, 512_B> m_sram{};
  bool has_battery;

  byte rom_bank = 1;
  bool ram_enabled = false;

public:
   mbc2(std::vector<byte> rom, bool has_battery);

  [[nodiscard]] byte readROM(const std::size_t index) const noexcept;
  void writeROM(const std::size_t index, const byte b) noexcept;

  [[nodiscard]] byte readSRAM(std::size_t index) const noexcept;
  void writeSRAM(std::size_t index, const byte b) noexcept;

  friend class Cartridge;
};

}
