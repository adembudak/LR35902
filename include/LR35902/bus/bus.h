#pragma once

#include <LR35902/config.h>

namespace LR35902 {

class Cartridge;
class PPU;
class BuiltIn;
class IO;
struct Interrupt;
class DMA;

class Bus {
  Cartridge &m_cart;
  PPU &m_ppu;
  BuiltIn &m_builtIn;
  DMA &m_dma;
  IO &m_io;

public:
  Interrupt &interruptHandler;

public:
  [[nodiscard]] Bus(Cartridge &cart, PPU &ppu, BuiltIn &builtIn, DMA &dma, IO &io, Interrupt &interrupt);

  [[nodiscard]] byte read(const std::size_t index) const noexcept;
  void write(const std::size_t index, const byte b) noexcept;

  void setPostBootValues() noexcept;
  void loadBootROM() noexcept;
};
}
