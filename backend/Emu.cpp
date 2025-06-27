#include <LR35902/config.h>
#include <backend/Emu.h>

bool Emu::tryBoot() noexcept {
  return bus.bootrom.load();
}

void Emu::skipBoot() noexcept {
  cpu.setPostBootValues();
  bus.setPostBootValues();
}

bool Emu::plug(const std::string &rom) noexcept {
  return cart.load(rom.data());
}

constexpr int vblank_period_cycles = 1140;

int Emu::step() noexcept {
  cpu.run();
  const int cycles = clock.latest();
  ppu.update(cycles);
  timer.update(cycles);

  return cycles;
}

void Emu::update() noexcept {
  while(ppu.mode() != lr::PPU::state::vblanking) {
    step();
  }

  int i = 0;
  while(i < vblank_period_cycles) {
    i += step();
  }
}

void Emu::reset() noexcept {
  cart.reset();
  ppu.reset();
  builtIn.reset();
  io.reset();
  intr.reset();
}

void Emu::resume() noexcept {
  m_state = state::running;
}

void Emu::stop() noexcept {
  m_state = state::stopped;
}

/*
bool GameBoy::onCreate() {}
bool GameBoy::onStart() {}
bool GameBoy::onResume() {}
bool GameBoy::onPause() {}
bool GameBoy::onStop() {}
bool GameBoy::onReset() {}
bool GameBoy::onDestroy() {}
*/
