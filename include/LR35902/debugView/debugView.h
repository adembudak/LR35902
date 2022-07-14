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

  bool _memory_portions = true;
  bool _disassembly = true;
  bool _cpu_state = true;
  bool _registers = true;
  bool _cartridge_header = true;

public:
  DebugView() = delete;

  DebugView(const Core &core, const IO &io, const Cartridge &cart, const PPU &ppu, const BuiltIn &builtIn) :
      m_core{core},
      m_io{io},
      m_cart{cart},
      m_ppu{ppu},
      m_builtIn{builtIn} {}

  void showMemoryPortions() noexcept;
  void showDisassembly() noexcept;
  void showCPUState() noexcept;
  void showRegisters() noexcept;
  void showCartridgeHeader() noexcept;
};

}
