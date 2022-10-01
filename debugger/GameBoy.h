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

#include <cstdint>
#include <functional>
#include <string_view>

enum class button : std::uint8_t {
  // direction keys
  right,
  left,
  up,
  down,

  // selection keys
  a,
  b,
  select,
  start,
};

constexpr button directionButtons[]{button::right, button::left, button::up, button::down};
constexpr button selectionButtons[]{button::a, button::b, button::select, button::start};

enum class keyStatus : std::uint8_t { pressed, released };

namespace lr = LR35902;

class [[nodiscard]] GameBoy final {
public:
  using screen_t = lr::PPU::screen_t;

private:
  lr::Cartridge cart;
  lr::BuiltIn builtIn;
  lr::IO io;
  lr::Interrupt intr;
  lr::PPU ppu{intr, io};

  lr::DMA dma{cart, ppu, builtIn};
  lr::Bus bus{cart, ppu, builtIn, dma, io, intr};
  lr::Timer timer{io, intr};
  lr::CPU cpu{bus};

  bool powered = false;
  bool paused = false;
  bool isRunning = false;

  void render() noexcept;

public:
  void setDrawCallback(const std::function<void(const screen_t &)> &drawCallback) noexcept;
  void skipboot(bool b = true) noexcept;
  void plug(const std::string_view rom) noexcept;
  void update() noexcept;
  void reset() noexcept;
  void pause() noexcept;
  void resume() noexcept;

  void stop() noexcept;
  bool isPowerOn() const noexcept;

  void joypad(button b, keyStatus s) noexcept;

  friend class lr::DebugView;
};
