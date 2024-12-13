#pragma once

#include <LR35902/config.h>

namespace LR35902 {

class Cartridge;
class PPU;
class BuiltIn;
class IO;
class Interrupt;
class DMA;
class Joypad;

class Bus {
  Cartridge &m_cart;
  PPU &m_ppu;
  BuiltIn &m_builtIn;
  DMA &m_dma;
  IO &m_io;
  Joypad &m_joypad;

public:
  Interrupt &interruptHandler;

public:
  [[nodiscard]] Bus(Cartridge &cart, PPU &ppu, BuiltIn &builtIn, DMA &dma, IO &io, Interrupt &interrupt,
                    Joypad &joypad);

  [[nodiscard]] byte read(const address_t index) const noexcept;
  void write(const address_t index, const byte b) noexcept;

  void setPostBootValues() noexcept;
  bool loadBootROM() noexcept;
};
}
