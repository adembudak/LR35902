#pragma once

#include <LR35902/config.h>

#include <array>
#include <vector>

namespace LR35902 {
struct MBC_config;

class rom_ram final {
  std::vector<byte> m_rom; // Usually 32KiB
  std::array<byte, 8_KiB> m_sram{};

  bool has_battery;

public:
  rom_ram(std::vector<byte> rom, const MBC_config& config);

  [[nodiscard]] byte readROM(const address_t index) const noexcept;
  void writeROM(const address_t index, const byte b) noexcept;

  [[nodiscard]] byte readSRAM(const address_t index) const noexcept;
  void writeSRAM(const address_t index, const byte b) noexcept;

  friend class Cartridge;
};

}
