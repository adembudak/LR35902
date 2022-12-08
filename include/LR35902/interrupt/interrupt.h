#pragma once

#include <LR35902/config.h>

#include <cstdint>

namespace LR35902 {

struct Interrupt {
  enum class kind : std::uint8_t { vblank, lcd_stat, timer, serial, joypad };

  byte IF{}; // 0xff0f, request
  byte IE{}; // 0xffff, enable (controlled by the game, not emu)

  [[nodiscard]] bool isThereAnAwaitingInterrupt() const noexcept;

  [[nodiscard]] kind get() const noexcept;
  void request(const kind k) noexcept;

  void reset() noexcept;
};

}
