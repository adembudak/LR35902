#pragma once

#include <LR35902/config.h>

#include <array>

namespace LR35902 {

class BuiltIn {
  std::array<byte, 8_KiB> m_wram{};
  std::array<byte, 7_KiB + 512_B> m_echo{};
  std::array<byte, 96_B> m_noUsable{};
  std::array<byte, 127_B> m_hram{};

public:

  [[nodiscard]] byte readWRAM(std::size_t index) const noexcept;
  void writeWRAM(std::size_t index, const byte b) noexcept;

  [[nodiscard]] byte readEcho(std::size_t index) const noexcept;
  void writeEcho(std::size_t index, const byte b) noexcept;

  [[nodiscard]] byte readNoUsable(const std::size_t index) const noexcept;
  void writeNoUsable(std::size_t index, const byte b) noexcept;

  [[nodiscard]] byte readHRAM(std::size_t index) const noexcept;
  void writeHRAM(std::size_t index, const byte b) noexcept;

  void reset() noexcept;

  friend class DMA;
  friend class DebugView;
};

}
