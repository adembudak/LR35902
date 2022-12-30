#include <LR35902/interrupt/interrupt.h>
#include <LR35902/io/io.h>
#include <LR35902/joypad/joypad.h>

namespace LR35902 {

// link: https://gbdev.io/pandocs/Joypad_Input.html
// link: http://www.codeslinger.co.uk/pages/projects/gameboy/joypad.html
// link: https://archive.org/details/GameBoyProgManVer1.1/page/n23/mode/1up

Joypad::Joypad(IO &io, Interrupt &intr) noexcept :
    m_io{io},
    m_intr{intr} {}

// clang-format off
/*
  Bit 7 - Not used
  Bit 6 - Not used
  Bit 5 - P15 Select Button Keys     (0=Select)
  Bit 4 - P14 Select Direction Keys  (0=Select)
  Bit 3 - P13 Input  Down  or Start  (0=Pressed) (Read Only)
  Bit 2 - P12 Input  Up    or Select (0=Pressed) (Read Only)
  Bit 1 - P11 Input  Left  or B      (0=Pressed) (Read Only)
  Bit 0 - P10 Input  Right or A      (0=Pressed) (Read Only)
*/

void Joypad::update(button btn, keystatus status) noexcept {

  const auto isDirectionButton = [] [[nodiscard]] (const button btn) -> bool {
    using enum button;
    return btn == up || btn == right || btn == down || btn == left;
  };

  const auto isSelectionButton = [] [[nodiscard]] (const button btn) -> bool {
    using enum button;
    return btn == a || btn == b || btn == select || btn == start;
  };

  const auto isPressed = [this] [[nodiscard]] (const button btn) -> bool {
    switch(btn) {
    case button::right:  return !(m_state & 0b0000'0001);
    case button::left:   return !(m_state & 0b0000'0010);
    case button::up:     return !(m_state & 0b0000'0100);
    case button::down:   return !(m_state & 0b0000'1000);
    case button::b:      return !(m_state & 0b0001'0000);
    case button::a:      return !(m_state & 0b0010'0000);
    case button::start:  return !(m_state & 0b0100'0000);
    case button::select: return !(m_state & 0b1000'0000);
    }
  };

  switch(status) {
  case keystatus::pressed: {

    bool isInterruptRequired = true;
    if(isPressed(btn)) // is the button already pressed before?
      isInterruptRequired = false;

    // clang-format off
    switch(btn) {
    case button::right:  m_state &= 0b1111'1110; break;
    case button::left:   m_state &= 0b1111'1101; break;
    case button::up:     m_state &= 0b1111'1011; break;
    case button::down:   m_state &= 0b1111'0111; break;
    case button::b:      m_state &= 0b1110'1111; break;
    case button::a:      m_state &= 0b1101'1111; break;
    case button::start:  m_state &= 0b1011'1111; break;
    case button::select: m_state &= 0b0111'1111; break;
    }

    const bool expectingDirectionPress = !(m_io.P1 & 0b0001'0000);
    const bool expectingSelectionPress = !(m_io.P1 & 0b0010'0000);

    if(isInterruptRequired)
      if((isDirectionButton(btn) && expectingDirectionPress) ||
         (isSelectionButton(btn) && expectingSelectionPress))
        m_intr.request(Interrupt::kind::joypad);

  } break;

    // clang-format off
  case keystatus::released:
    switch(btn) {
    case button::right:  m_state |= 0b0000'0001; break;
    case button::left:   m_state |= 0b0000'0010; break;
    case button::up:     m_state |= 0b0000'0100; break;
    case button::down:   m_state |= 0b0000'1000; break;
    case button::b:      m_state |= 0b0001'0000; break;
    case button::a:      m_state |= 0b0010'0000; break;
    case button::start:  m_state |= 0b0100'0000; break;
    case button::select: m_state |= 0b1000'0000; break;
    }
  }
}

byte Joypad::read() const noexcept {
  byte result = m_io.P1 ^ 0xff;

  const bool expectingDirectionPress = !(m_io.P1 & 0b0001'0000);

  if(expectingDirectionPress) {
    const byte topJoypad = ((m_state & 0xf0) >> 4) | 0xf0;
    result &= topJoypad;
    return result;
  }

  else { 
    const byte bottomJoypad = (m_state & 0x0f) | 0xf0;
    result &= bottomJoypad;
    return result;
  }
}

bool Joypad::isBlocked() const noexcept {
  return (m_io.P1 & 0b1100'0000) == 0b11;
}

}
