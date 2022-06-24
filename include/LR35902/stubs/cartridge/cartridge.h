#pragma once

#include <LR35902/config.h>

#include <array>
#include <vector>

namespace LR35902 {

class Cartridge {
private:
  std::vector<byte> m_rom{};
  std::array<byte, 8_KiB> m_sram{};

public:
  void load(const char *romfile);

  byte &operator[](const std::size_t index) noexcept;

  [[nodiscard]] byte readSRAM(const std::size_t index) const noexcept;
  void writeSRAM(const std::size_t index, const byte b) noexcept;

  [[nodiscard]] byte readROM(const std::size_t index) const noexcept;
  void writeROM(const std::size_t index, const byte b) noexcept;

  [[nodiscard]] auto data() const noexcept -> std::vector<byte>;

public:
  friend class DebugView;
};
}
