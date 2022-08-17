#include "GameBoy.h"

#include <string_view>

namespace LR35902 {

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

void GameBoy::play() noexcept {
  if(!paused) {
    cpu.run();
    ppu.update(cpu.latestCycles());
    timer.update(cpu.latestCycles());
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
}
