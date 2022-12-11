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
  return m_isRunning;
}
