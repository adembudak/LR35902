#include <LR35902/config.h>
#include <LR35902/stubs/ppu/ppu.h>

#include <cstddef>

namespace LR35902 {

byte PPU::readVRAM(const std::size_t index) const noexcept {
  return m_vram[index];
}

void PPU::writeVRAM(const std::size_t index, const byte b) noexcept {
  m_vram[index] = b;
}

byte PPU::readOAM(const std::size_t index) const noexcept {
  return m_oam[index];
}

void PPU::writeOAM(const std::size_t index, const byte b) noexcept {
  m_oam[index] = b;
}

}
