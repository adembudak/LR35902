#include <LR35902/config.h>
#include <backend/GameBoy.h>

bool Emu::tryBoot() noexcept {
  return bus.loadBootROM();
}

void Emu::skipBoot() noexcept {
  cpu.setPostBootValues();
  bus.setPostBootValues();
}

bool Emu::plug(const std::string &rom) noexcept {
  return cart.load(rom.data());
}

constexpr int vblank_period_cycles = 1140;

void Emu::update() noexcept {
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
