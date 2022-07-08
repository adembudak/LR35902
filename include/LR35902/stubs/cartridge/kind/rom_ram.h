#pragma once

#include <LR35902/config.h>

#include <array>

namespace LR35902 {

class rom_ram {
  std::array<byte, 32_KiB> m_rom{};
  std::array<byte, 8_KiB> m_sram{};

public:
  explicit rom_ram(auto begin, auto end) {
    std::move(begin, end, m_rom.begin());
  }

  [[nodiscard]] byte readROM(const std::size_t index) const noexcept;
  void writeROM(const std::size_t index, const byte b) noexcept;

  [[nodiscard]] byte readSRAM(const std::size_t index) const noexcept;
  void writeSRAM(const std::size_t index, const byte b) noexcept;

  [[nodiscard]] auto data() const noexcept -> decltype(m_rom);
};

}
