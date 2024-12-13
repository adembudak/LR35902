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

  [[nodiscard]] byte readWRAM(address_t index) const noexcept;
  void writeWRAM(address_t index, const byte b) noexcept;

  [[nodiscard]] byte readEcho(address_t index) const noexcept;
  void writeEcho(address_t index, const byte b) noexcept;

  [[nodiscard]] byte readNoUsable(address_t index) const noexcept;
  void writeNoUsable(address_t index, const byte b) noexcept;

  [[nodiscard]] byte readHRAM(address_t index) const noexcept;
  void writeHRAM(address_t index, const byte b) noexcept;

  void reset() noexcept;

  friend class DMA;
  friend class DebugView;
};

}
