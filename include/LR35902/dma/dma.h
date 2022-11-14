#pragma once

#include <LR35902/config.h>
#include <LR35902/memory_map.h>

#include <cstddef>

namespace LR35902 {

// direct memory access
class Cartridge;
class PPU;
class BuiltIn;
class Clock;

class DMA {
  Cartridge &m_cart;
  PPU &m_ppu;
  BuiltIn &m_builtIn;
  Clock &m_clock;

public:
  DMA(Cartridge &cart, PPU &ppu, BuiltIn &builtIn, Clock &clock);
  void action(const byte n) noexcept;
};

}
