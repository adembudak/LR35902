#pragma once

#include <LR35902/config.h>

#include <array>

namespace LR35902 {

class Cartridge;
class PPU;
class BuiltIn;
class IO;
class Interrupt;

class Bus {
  Cartridge &m_cart;
  PPU &m_ppu;
  BuiltIn &m_builtIn;
  IO &m_io;

public:
  Interrupt &interruptHandler;

public:
  [[nodiscard]] Bus(Cartridge &cart, PPU &ppu, BuiltIn &builtIn, IO &io, Interrupt &interrupt);

  [[nodiscard]] byte read(const std::size_t index) const noexcept;
  void write(const std::size_t index, const byte b) noexcept;

  [[nodiscard]] byte &read_write(const std::size_t index) noexcept;
};
}
