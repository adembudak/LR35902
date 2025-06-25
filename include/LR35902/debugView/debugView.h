#pragma once

#include <LR35902/cpu/cpu.h>


#include <imgui.h>
#include <imgui_memory_editor.h>

#include <cstdint>
#include <map>

class Emu;

namespace LR35902 {

class DebugView {
  std::map<std::uint16_t, std::pair<byte, CPU::immediate_t>> iv;
  const Emu &emu;

  MemoryEditor memory_editor;

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

  bool _header = true;
  bool _disassembly = true;
  bool _cpu_state = true;
  bool _registers = true;
  bool _vram = false;

public:
  DebugView() = delete;
  DebugView(const Emu &);

  ~DebugView() = default;

  DebugView(const DebugView &) = delete;
  DebugView &operator=(const DebugView &) = delete;

  DebugView(DebugView &&) = delete;
  DebugView &operator=(DebugView &&) = delete;

  void showCartHeader() noexcept;
  void showMemoryPortions() noexcept;
  void showDisassembly() noexcept;
  void showCPUState() noexcept;
  void showRegisters() noexcept;
};

}
