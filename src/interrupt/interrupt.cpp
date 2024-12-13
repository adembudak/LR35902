#include <LR35902/interrupt/interrupt.h>
#include <LR35902/io/io.h>

#ifdef __clang__
  #pragma GCC diagnostic push
  #pragma GCC diagnostic ignored "-Wreturn-type"
#endif

namespace LR35902 {

Interrupt::Interrupt(IO &io) :
    m_io{io} {}

bool Interrupt::isThereAnAwaitingInterrupt() const noexcept {
  return _IE & m_io.IF & 0b0001'1111;
}

byte Interrupt::IE() const noexcept {
  return _IE;
}

void Interrupt::IE(const byte b) noexcept {
  _IE = b;
}

bool Interrupt::isThereAnEnabledInterrupt() const noexcept {
  return _IE;
}

Interrupt::kind Interrupt::get() const noexcept {
  if(_IE & m_io.IF & 0b0000'0001) return kind::vblank;
  if(_IE & m_io.IF & 0b0000'0010) return kind::lcd_stat;
  if(_IE & m_io.IF & 0b0000'0100) return kind::timer;
  if(_IE & m_io.IF & 0b0000'1000) return kind::serial;
  if(_IE & m_io.IF & 0b0001'0000) return kind::joypad;
}

// clang-format off
void Interrupt::request(const kind k) noexcept {
  switch(k) {
  case kind::vblank:   m_io.IF |= 0b0000'0001; break;
  case kind::lcd_stat: m_io.IF |= 0b0000'0010; break;
  case kind::timer:    m_io.IF |= 0b0000'0100; break;
  case kind::serial:   m_io.IF |= 0b0000'1000; break;
  case kind::joypad:   m_io.IF |= 0b0001'0000; break;
  }
}

void Interrupt::serve(const kind k) noexcept {
  switch(k) {
  case kind::vblank:   m_io.IF &= 0b1111'1110; break;
  case kind::lcd_stat: m_io.IF &= 0b1111'1101; break;
  case kind::timer:    m_io.IF &= 0b1111'1011; break;
  case kind::serial:   m_io.IF &= 0b1111'0111; break;
  case kind::joypad:   m_io.IF &= 0b1110'1111; break;
  }
}
// clang-format on

void Interrupt::reset() noexcept {
  m_io.IF = _IE = byte{};
}

}

#ifdef __clang__
#pragma GCC diagnostic pop
#endif
