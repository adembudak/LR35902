#pragma once

#include <LR35902/builtin/builtin.h>
#include <LR35902/cartridge/cartridge.h>
#include <LR35902/cpu/cpu.h>
#include <LR35902/dma/dma.h>
#include <LR35902/interrupt/interrupt.h>
#include <LR35902/io/io.h>
#include <LR35902/joypad/joypad.h>
#include <LR35902/ppu/ppu.h>
#include <LR35902/timer/timer.h>

#include <string>

namespace lr = LR35902;

struct Emu {
  enum state { stopped, running };
  state m_state;

  lr::IO io;
  lr::Interrupt intr{io};
  lr::Joypad joypad{io, intr};
  lr::PPU ppu{intr, io};

  lr::Clock clock;
  lr::Cartridge cart;
  lr::BuiltIn builtIn;

  lr::DMA dma{cart, ppu, builtIn, clock};
  lr::Bus bus{cart, ppu, builtIn, dma, io, intr, joypad};
  lr::Timer timer{io, intr};
  lr::CPU cpu{bus, clock};

  bool tryBoot() noexcept;
  void skipBoot() noexcept;
  bool plug(const std::string &rom) noexcept;

  void update() noexcept;

  void reset() noexcept;
  void resume() noexcept;
  void stop() noexcept;

#if defined(WITH_DEBUGGER)
  friend class LR35902::DebugView;
#endif
};
