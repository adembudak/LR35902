#pragma once

namespace LR35902 {

class Core;
class Cartridge;

class DebugView {
public:
  static void registerStatus(const Core &core) noexcept;
  static void romDump(const Cartridge &cart) noexcept;
};

}
