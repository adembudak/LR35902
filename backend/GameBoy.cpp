#include <backend/GameBoy.h>
#include <LR35902/config.h>

#include <string_view>

bool GameBoy::tryBoot() noexcept {
  return bus.loadBootROM();
}

void GameBoy::skipboot() noexcept {
  cpu.setPostBootValues();
  bus.setPostBootValues();
}

bool GameBoy::plug(const std::string &rom) noexcept {
  return cart.load(rom.data());
}

constexpr int vblank_period_cycles = 1140;

void GameBoy::update() noexcept {
//  if(m_paused) return;

  while(ppu.mode() != lr::PPU::state::vblanking) {
    cpu.run();
    const int cycles = clock.latest();
    ppu.update(cycles);
    timer.update(cycles);
  }

  int cycles = 0;
  for(int i = 0; i < vblank_period_cycles; i += cycles) {
    cpu.run();
    cycles = clock.latest();
    ppu.update(cycles);
    timer.update(cycles);
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
