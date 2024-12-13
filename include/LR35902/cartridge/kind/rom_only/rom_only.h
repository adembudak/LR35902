#pragma once

#include <LR35902/config.h>

#include <cstdint>
#include <vector>

namespace LR35902 {

class rom_only final {
  std::vector<byte> m_rom;

public:
  explicit rom_only() = default;

  explicit rom_only(std::vector<byte> rom) :
      m_rom(std::move(rom)) {}

  [[nodiscard]] byte readROM(const address_t index) const noexcept;
  void writeROM(const address_t index, const byte b) noexcept;

  friend class Cartridge;
};

}
