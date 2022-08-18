#include "GameBoy.h"
#include <LR35902/config.h>

#include <algorithm>
#include <cassert>
#include <string_view>

void GameBoy::boot(bool skipboot) noexcept {
  isRunning = true;

  if(!skipboot) {
    cpu.setPostBootValues();
    bus.setPostBootValues();
  } else {
    // implement this:
    // ifstream take boot rom from file
    // insert it into the cartridge
  }
}

void GameBoy::plug(const std::string_view rom) noexcept {
  cart.load(rom.data());
}

// constexpr std::size_t base_clock = 1'048'576;
// constexpr std::size_t base_clock_per_second = 1'048'576 / 60;

void GameBoy::play() noexcept {
  //  std::size_t i = 0;
  if(!paused) {
    //   while(i < base_clock) {
    cpu.run();
    const auto cycles = cpu.latestCycles();
    ppu.update(cycles);
    timer.update(cycles);
    //   i += cycles;
    //  }
  }
}

void GameBoy::reset() noexcept {
  cart.reset();
  ppu.reset();
  builtIn.reset();
  io.reset();
  intr.reset();
}

void GameBoy::pause() noexcept {
  paused = true;
}

void GameBoy::resume() noexcept {
  paused = false;
}

void GameBoy::stop() noexcept {
  isRunning = false;
}

bool GameBoy::isPowerOn() const noexcept {
  return isRunning;
}

// link: https://gbdev.io/pandocs/Joypad_Input.html
// link: http://www.codeslinger.co.uk/pages/projects/gameboy/joypad.html
// link: https://archive.org/details/GameBoyProgManVer1.1/page/n23/mode/1up
//
// 0: button pressed, 1: button released
//
// REVISIT: this probably shouldn't be this hard, maybe i'm overthinking
void GameBoy::joypad(button btn, keyStatus s) noexcept {
  const std::uint8_t buttonKind = (io.P1 & 0b0011'0000) >> 4;
  const bool isPressed = s == keyStatus::pressed;

  // direction button
  // the implementation expects a direction button to be pressed. If pressed button is not one of them,
  // so skip it.
  if(isPressed && buttonKind == 0b01 &&
     // #ifdef __cpp_lib_ranges
     //     std::ranges::none_of(selectionButtons, [=](button b) { return btn == b; }))
     // #else
     std::none_of(std::begin(selectionButtons), std::end(selectionButtons),
                  [=](button b) { return btn == b; }))
    //#endif
    return;

  // selection button
  if(isPressed && buttonKind == 0b10 &&
     //#ifdef __cpp_lib_ranges
     //     std::none_of(directionButtons, [=](button b) { return btn == b; }))
     //#else
     std::none_of(std::begin(directionButtons), std::end(directionButtons),
                  [=](button b) { return btn == b; }))
    //#endif
    return;

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
