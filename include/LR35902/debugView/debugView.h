#pragma once

namespace LR35902 {

class Core;
class Cartridge;
class PPU;
class BuiltIn;
class IO;
class Interrupt;

class DebugView {
  const Core &m_core;
  const IO &m_io;
  const Cartridge &m_cart;
  const PPU &m_ppu;
  const BuiltIn &m_builtIn;

public:
  void disassembly() const noexcept;
  void CPURegisters() const noexcept;
  void registers() const noexcept;

  void dumpROM() const noexcept;
  void dumpSRAM() const noexcept;

  void dumpVRAM() const noexcept;
  void dumpOAM() const noexcept;

  void dumpWRAM() const noexcept;
  void dumpHRAM() const noexcept;
};

}
