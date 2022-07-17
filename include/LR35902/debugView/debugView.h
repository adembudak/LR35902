#pragma once

namespace LR35902 {

class Core;
class Cartridge;
class PPU;
class BuiltIn;
class IO;
class Interrupt;

class DebugView {
private:
  const Core &m_core;
  const IO &m_io;
  const Cartridge &m_cart;
  const PPU &m_ppu;
  const BuiltIn &m_builtIn;

public:
  bool _memory_portions = true;
  bool _memory_portions_rom = true;
  bool _memory_portions_vram = true;
  bool _memory_portions_sram{};
  bool _memory_portions_wram = true;
  bool _memory_portions_echo = false;
  bool _memory_portions_oam = true;
  bool _memory_portions_noUsable = false;
  bool _memory_portions_io = true;
  bool _memory_portions_hram = true;

  bool _disassembly = true;
  bool _cpu_state = true;
  bool _registers = true;
  bool _cartridge_header = true;

public:
  DebugView() = delete;
  DebugView(const Core &core, const IO &io, const Cartridge &cart, const PPU &ppu, const BuiltIn &builtIn);

  ~DebugView() = default;

  DebugView(const DebugView &) = delete;
  DebugView &operator=(const DebugView &) = delete;

  DebugView(DebugView &&) = delete;
  DebugView &operator=(DebugView &&) = delete;

  void showMemoryPortions() noexcept;
  void showDisassembly() noexcept;
  void showCPUState() noexcept;
  void showRegisters() noexcept;
  void showCartridgeHeader() noexcept;
};

}
