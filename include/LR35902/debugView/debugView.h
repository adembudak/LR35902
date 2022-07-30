#pragma once

#include <LR35902/core/core.h>

#include <cstdint>
#include <map>
#include <optional>
#include <tuple>

namespace LR35902 {

class GameBoy;

class DebugView {
  const GameBoy &gameboy;

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

  using Operation = std::tuple<Core::OpcodeKind, std::uint8_t, std::optional<std::uint16_t>>;
  std::map<std::uint16_t, Operation> instructions; // pc -> opcodeKind, opcode, immediate

public:
  DebugView() = delete;
  DebugView(const GameBoy &);

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
