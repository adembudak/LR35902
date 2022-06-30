#pragma once

namespace LR35902 {

class Core;
class Cartridge;
class PPU;
class BuiltIn;
class IO;

class DebugView {
public:
  static void CPURegisters(const Core &core) noexcept;
  static void registers(const IO &io);

  static void dumpROM(const Cartridge &cart) noexcept;
  static void dumpSRAM(const Cartridge &cart) noexcept;

  static void dumpVRAM(const PPU &ppu) noexcept;
  static void dumpOAM(const PPU &ppu) noexcept;

  static void dumpWRAM(const BuiltIn &builtin) noexcept;
  static void dumpHRAM(const BuiltIn &builtin) noexcept;
};

}
