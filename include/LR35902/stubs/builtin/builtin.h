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

  [[nodiscard]] byte readWRAM(const std::size_t index) const noexcept;
  void writeWRAM(const std::size_t index, const byte b) noexcept;

  [[nodiscard]] byte readEcho(const std::size_t index) const noexcept;
  void writeEcho(const std::size_t index, const byte b) noexcept;

  [[nodiscard]] byte readNoUsable(const std::size_t index) const noexcept;
  void writeNoUsable(const std::size_t index, const byte b) noexcept;

  [[nodiscard]] byte readHRAM(const std::size_t index) const noexcept;
  void writeHRAM(const std::size_t index, const byte b) noexcept;

  [[nodiscard]] byte &operator[](const std::size_t index) noexcept;

  friend class DebugView;
};

}
