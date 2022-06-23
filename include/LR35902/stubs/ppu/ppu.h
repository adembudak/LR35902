#pragma once

#include <LR35902/config.h>

#include <array>

namespace LR35902 {

class PPU {
  std::array<byte, 8_KiB> m_vram{};
  std::array<byte, 160_B> m_oam{};

public:

  [[nodiscard]] byte readVRAM(const std::size_t index) const noexcept;
  void writeVRAM(const std::size_t index, const byte b) noexcept;

  [[nodiscard]] byte readOAM(const std::size_t index) const noexcept;
  void writeOAM(const std::size_t index, const byte b) noexcept;

  friend class DebugView;
};

}
