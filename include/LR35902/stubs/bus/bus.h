#pragma once

#include <LR35902/config.h>

#include <array>

namespace LR35902 {

class Cartridge;
class PPU;
class BuiltIn;

class Bus {
private:
  Cartridge &m_cart;
  PPU &m_ppu;
  BuiltIn &m_builtIn;

public:
  [[nodiscard]] Bus(Cartridge &cart, PPU &ppu, BuiltIn &builtIn);

  [[nodiscard]] byte &read_write(const std::size_t index) noexcept;
  [[nodiscard]] byte read(const std::size_t index) const noexcept;
  void write(const std::size_t index, const byte b) noexcept;
};
}
