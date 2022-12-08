#pragma once

#include <LR35902/config.h>

#include <cstdint>

namespace LR35902 {

class Interrupt {
public:
  enum class kind : std::uint8_t { vblank, lcd_stat, timer, serial, joypad };

private:
  byte _IF{}; // 0xff0f, interrupt request
  byte _IE{}; // 0xffff, interrupt enable (controlled by the game dev, not emu)

public:
  [[nodiscard]] bool isThereAnAwaitingInterrupt() const noexcept;

  [[nodiscard]] byte IF() const noexcept;
  void IF(const byte b) noexcept;

  [[nodiscard]] byte IE() const noexcept;
  void IE(const byte b) noexcept;

  [[nodiscard]] kind get() const noexcept;

  void request(const kind k) noexcept;
  void serve(const kind k) noexcept;

  void reset() noexcept;

  friend class DebugView;
};

}
