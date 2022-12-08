#include <LR35902/interrupt/interrupt.h>

namespace LR35902 {

[[nodiscard]] bool Interrupt::isThereAnAwaitingInterrupt() const noexcept {
  return _IE & _IF & 0b0001'1111;
}

byte Interrupt::IF() const noexcept {
  return _IF;
}

void Interrupt::IF(const byte b) noexcept {
  _IF = b;
}

byte Interrupt::IE() const noexcept {
  return _IE;
}

void Interrupt::IE(const byte b) noexcept {
  _IE = b;
}

// clang-format off
[[nodiscard]] Interrupt::kind Interrupt::get() const noexcept {
  if     (_IE & _IF & 0b0000'0001) return kind::vblank;
  else if(_IE & _IF & 0b0000'0010) return kind::lcd_stat;
  else if(_IE & _IF & 0b0000'0100) return kind::timer;
  else if(_IE & _IF & 0b0000'1000) return kind::serial;
  else if(_IE & _IF & 0b0001'0000) return kind::joypad;
}

void Interrupt::request(const kind k) noexcept {
  switch(k) {
  case kind::vblank:   _IF |= 0b0000'0001; break;
  case kind::lcd_stat: _IF |= 0b0000'0010; break;
  case kind::timer:    _IF |= 0b0000'0100; break;
  case kind::serial:   _IF |= 0b0000'1000; break;
  case kind::joypad:   _IF |= 0b0001'0000; break;
  };
}

void Interrupt::serve(const kind k) noexcept {
  // clang-format off
  switch(k) {
  case kind::vblank:   _IF &= 0b1111'1110; break;
  case kind::lcd_stat: _IF &= 0b1111'1101; break;
  case kind::timer:    _IF &= 0b1111'1011; break;
  case kind::serial:   _IF &= 0b1111'0111; break;
  case kind::joypad:   _IF &= 0b1110'1111; break;
    // clang-format on
  }
}

void Interrupt::reset() noexcept {
  _IF = _IE = byte{};
}

}
