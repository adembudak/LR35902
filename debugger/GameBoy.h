#pragma once

#include <LR35902/core/core.h>
#include <LR35902/stubs/builtin/builtin.h>
#include <LR35902/stubs/cartridge/cartridge.h>
#include <LR35902/stubs/interrupt/interrupt.h>
#include <LR35902/stubs/io/io.h>
#include <LR35902/stubs/ppu/ppu.h>

#include <string_view>

namespace LR35902 {

class [[nodiscard]] GameBoy final {
  Cartridge cart;
  PPU ppu;
  BuiltIn builtIn;
  IO io;
  Interrupt intr;

  Bus bus{cart, ppu, builtIn, io, intr};
  Core core{bus};

  void render() noexcept;

  bool powered = false;
  bool paused = false;
  bool isRunning = false;

public:

  std::size_t updateCycles() const noexcept {
    return core.latestCycles();
  }
  void boot(bool skipboot = true) noexcept;

  void plug(const std::string_view rom) noexcept;
  void play() noexcept;
  void reset() noexcept;
  void pause() noexcept;
  void resume() noexcept;

  void stop() noexcept;
  bool isPowerOn() const noexcept;

  friend class DebugView;
};

}
