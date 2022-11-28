#pragma once

#include <LR35902/config.h>

#include <cstdint>

namespace LR35902 {

struct Interrupt {
  enum class kind : std::uint8_t { vblank, lcd_stat, timer, serial, joypad };

  byte IF{}; // 0xff0f, request
  byte IE{}; // 0xffff, enable (controlled by the game, not emu)

  [[nodiscard]] bool isThereAnAwaitingInterrupt() const noexcept {
    return IE & IF & 0b0001'1111;
  }

  // clang-format off
  [[nodiscard]] kind get() const noexcept {
    if     (IE & IF & 0b0000'0001) return kind::vblank;
    else if(IE & IF & 0b0000'0010) return kind::lcd_stat;
    else if(IE & IF & 0b0000'0100) return kind::timer;
    else if(IE & IF & 0b0000'1000) return kind::serial;
    else if(IE & IF & 0b0001'0000) return kind::joypad;
  }

  void request(const kind k) noexcept {
    switch(k) {
    case kind::vblank:   IF |= 0b0000'0001; break;
    case kind::lcd_stat: IF |= 0b0000'0010; break;
    case kind::timer:    IF |= 0b0000'0100; break;
    case kind::serial:   IF |= 0b0000'1000; break;
    case kind::joypad:   IF |= 0b0001'0000; break;
    };
  }

  void reset() noexcept {
    IF = IE = byte{};
  }
};

}
