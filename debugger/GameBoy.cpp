#include "GameBoy.h"
#include <LR35902/config.h>

#include <algorithm>
#include <cassert>
#include <string_view>

void GameBoy::setDrawCallback(const std::function<void(const screen_t &)> &drawCallback) noexcept {
  ppu.setDrawCallback(drawCallback);
}

void GameBoy::skipboot(bool b) noexcept {
  m_isRunning = true;

  if(b) {
    cpu.setPostBootValues();
    bus.setPostBootValues();
  } else {
    bus.loadBootROM();
  }
}

void GameBoy::plug(const std::string_view rom) noexcept {
  cart.load(rom.data());
}

void GameBoy::update() noexcept {
  if(!m_paused) {
    const int one_scanline_period = 114 * 144;
    int cycles = 0;
    for(int i = 0; i < one_scanline_period; i += cycles) {
      cpu.run();
      cycles = clock.latest();
      ppu.update(cycles);
      timer.update(cycles);
    }
  }
}

void GameBoy::reset() noexcept {
  cart.reset();
  ppu.reset();
  builtIn.reset();
  io.reset();
  intr.reset();
}

bool GameBoy::isPaused() noexcept {
  return m_paused;
}

void GameBoy::pause() noexcept {
  m_paused = true;
}

void GameBoy::resume() noexcept {
  m_paused = false;
}

void GameBoy::stop() noexcept {
  m_isRunning = false;
}

bool GameBoy::isPowerOn() const noexcept {
  return m_isRunning == true;
}

// link: https://gbdev.io/pandocs/Joypad_Input.html
// link: http://www.codeslinger.co.uk/pages/projects/gameboy/joypad.html
// link: https://archive.org/details/GameBoyProgManVer1.1/page/n23/mode/1up
//
// 0: button pressed, 1: button released
void GameBoy::joypad(button btn, keyStatus s) noexcept {
  const std::uint8_t buttonKind = (io.P1 & 0b0011'0000) >> 4;
  const bool isPressed = s == keyStatus::pressed;

  if(isPressed) {
    // selection button
    // Implementation expects a selection button to be pressed. If the pressed button isn't one of them,
    // skip it
    if(buttonKind == 0b01 &&
       std::none_of(begin(selectionButtons), end(selectionButtons), [=](button b) { return btn == b; }))
      return;

    // direction button
    if(buttonKind == 0b10 &&
       std::none_of(begin(directionButtons), end(directionButtons), [=](button b) { return btn == b; }))
      return;
  }

  switch(btn) {
    // clang-format off
  case button::a:      isPressed ? io.P1 &= 0b1111'1110 : io.P1 |= 0b0000'0001; break;
  case button::b:      isPressed ? io.P1 &= 0b1111'1101 : io.P1 |= 0b0000'0010; break;
  case button::select: isPressed ? io.P1 &= 0b1111'1011 : io.P1 |= 0b0000'0100; break;
  case button::start:  isPressed ? io.P1 &= 0b1111'0111 : io.P1 |= 0b0000'1000; break;

  case button::right:  isPressed ? io.P1 &= 0b1111'1110 : io.P1 |= 0b0000'0001; break;
  case button::left:   isPressed ? io.P1 &= 0b1111'1101 : io.P1 |= 0b0000'0010; break;
  case button::up:     isPressed ? io.P1 &= 0b1111'1011 : io.P1 |= 0b0000'0100; break;
  case button::down:   isPressed ? io.P1 &= 0b1111'0111 : io.P1 |= 0b0000'1000; break;
    // clang-format on
  }

  if(isPressed) intr.request(lr::Interrupt::kind::joypad);
}
