#pragma once

#include <LR35902/config.h>

#include <cstddef>
#include <vector>

namespace LR35902 {

class mbc3 final {
  std::vector<byte> m_rom;
  std::vector<byte> m_sram{};

  byte RAM_enabled = 0;
  byte ROM_bank = 1;
  byte SRAM_bank = 0;
  byte latch = 0;

  struct RTC_t {
    byte seconds;
    byte minutes;
    byte hours;
    byte days_lo;
    byte days_hi;
  };

  RTC_t RTC;

public:
  explicit mbc3(std::vector<byte> rom);

  [[nodiscard]] byte readROM(const std::size_t index) const noexcept;
  void writeROM(const std::size_t index, const byte b) noexcept;

  [[nodiscard]] byte readSRAM(const std::size_t index) const noexcept;
  void writeSRAM(const std::size_t index, const byte b) noexcept;

  friend class Cartridge;
};

}
