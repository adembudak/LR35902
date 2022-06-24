#pragma once

#include <LR35902/config.h>

#include <array>

namespace LR35902 {

class Cartridge;
class PPU;
class BuiltIn;
class IO;

class Bus {
  Cartridge &m_cart;
  PPU &m_ppu;
  BuiltIn &m_builtIn;
  IO &m_io;

public:
  [[nodiscard]] Bus(Cartridge &cart, PPU &ppu, BuiltIn &builtIn, IO &io);

  [[nodiscard]] byte &read_write(const std::size_t index) noexcept;

  [[nodiscard]] byte read(const std::size_t index) const noexcept;
  void write(const std::size_t index, const byte b) noexcept;
};
}
