#pragma once

#include <LR35902/builtin/builtin.h>
#include <LR35902/cartridge/cartridge.h>
#include <LR35902/cpu/cpu.h>
#include <LR35902/debugView/debugView.h>
#include <LR35902/dma/dma.h>
#include <LR35902/interrupt/interrupt.h>
#include <LR35902/io/io.h>
#include <LR35902/ppu/ppu.h>
#include <LR35902/timer/timer.h>

#include <string_view>

namespace lr = LR35902;

class [[nodiscard]] GameBoy final {
  lr::Cartridge cart;
  lr::PPU ppu;
  lr::BuiltIn builtIn;
  lr::IO io;
  lr::Interrupt intr;

  lr::Joypad joypad;
  lr::DMA dma{cart, ppu, builtIn};
  lr::Bus bus{cart, ppu, builtIn, dma, io, intr};
  lr::Timer timer{io, intr};
  lr::CPU cpu{bus};

  bool powered = false;
  bool paused = false;
  bool isRunning = false;

  void render() noexcept;

public:
  void boot(bool skipboot = true) noexcept;

  void plug(const std::string_view rom) noexcept;
  void play() noexcept;
  void reset() noexcept;
  void pause() noexcept;
  void resume() noexcept;

  void stop() noexcept;
  bool isPowerOn() const noexcept;

  friend class lr::DebugView;
};
