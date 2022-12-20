#pragma once

#include <LR35902/builtin/builtin.h>
#include <LR35902/cartridge/cartridge.h>
#include <LR35902/cpu/cpu.h>
#include <LR35902/debugView/debugView.h>
#include <LR35902/dma/dma.h>
#include <LR35902/interrupt/interrupt.h>
#include <LR35902/io/io.h>
#include <LR35902/joypad/joypad.h>
#include <LR35902/ppu/ppu.h>
#include <LR35902/timer/timer.h>

#include <cstdint>
#include <functional>
#include <string_view>

namespace lr = LR35902;

class [[nodiscard]] GameBoy final {
public:
  using screen_t = lr::PPU::screen_t;

  lr::Clock clock;
  lr::Cartridge cart;
  lr::BuiltIn builtIn;
  lr::IO io;
  lr::Interrupt intr{io};
  lr::Joypad joypad{io, intr};
  lr::PPU ppu{intr, io};

  lr::DMA dma{cart, ppu, builtIn, clock};
  lr::Bus bus{cart, ppu, builtIn, dma, io, intr, joypad};
  lr::Timer timer{io, intr};
  lr::CPU cpu{bus, clock};

  mutable bool m_power = false;
  mutable bool m_paused = false;
  mutable bool m_isRunning = false;

  void skipboot(bool b = true) noexcept;
  void plug(const std::string_view rom) noexcept;

  void update() noexcept;

  void reset() noexcept;
  bool isPaused() noexcept;
  void pause() noexcept;
  void resume() noexcept;

  void stop() noexcept;
  bool isPowerOn() const noexcept;

  friend class lr::DebugView;
};
