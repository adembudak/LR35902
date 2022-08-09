#pragma once

#include <LR35902/memory_map.h>

#include <cstddef>

namespace LR35902 {

// direct memory access
class Cartridge;
class PPU;
class BuiltIn;

class DMA {
  Cartridge &m_cart;
  PPU &m_ppu;
  BuiltIn &m_builtIn;

public:
  DMA(Cartridge &cart, PPU &ppu, BuiltIn &builtIn);
  void action(const byte n) noexcept;
};

}
