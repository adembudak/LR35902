#pragma once

#include <LR35902/config.h>

#include <cstdint>

namespace LR35902 {

class IO;

class Interrupt {
public:
  enum class kind : std::uint8_t { vblank, lcd_stat, timer, serial, joypad };

private:
  IO &m_io;
  byte _IE{};

public:
  explicit Interrupt(IO &io);

  [[nodiscard]] bool isThereAnAwaitingInterrupt() const noexcept;

  [[nodiscard]] byte IE() const noexcept;
  void IE(const byte b) noexcept;

  [[nodiscard]] bool isThereAnEnabledInterrupt() const noexcept;

  [[nodiscard]] kind get() const noexcept;

  void request(const kind k) noexcept;
  void serve(const kind k) noexcept;

  void reset() noexcept;

  friend class DebugView;
};

}
