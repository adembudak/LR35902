#include <LR35902/config.h>
#include <LR35902/interrupt/interrupt.h>
#include <LR35902/io/io.h>
#include <LR35902/memory_map.h>
#include <LR35902/ppu/ppu.h>

#include <cassert>
#include <cstddef>

namespace LR35902 {

bool PPU::isLCDEnabled() const noexcept { // bit7
  return LCDC & 0b1000'0000;
}

std::size_t PPU::windowTileMapIndex() const noexcept { // bit6
  return (LCDC & 0b0100'0000) ? 0x1C00 : 0x1800;
}

// REVISIT: this effected by bit0
bool PPU::isWindowEnabled() const noexcept { // bit5
  return LCDC & 0b0010'0000;
}

std::size_t PPU::backgroundTilesetIndex() const noexcept { // bit4
  return LCDC & 0b0001'0000;
}

// window and background share same space, so this does same thing above
std::size_t PPU::windowTilesetIndex() const noexcept { // bit4
  return LCDC & 0b0001'0000;
}

std::size_t PPU::backgroundTilemapIndex() const noexcept { // bit3
  return (LCDC & 0b0000'1000) ? 0x1C00 : 0x1800;
}

bool PPU::isBigSprite() const noexcept { // bit2
  return LCDC & 0b0000'0100;
}

bool PPU::isSpritesEnabled() const noexcept { // bit1
  return LCDC & 0b0000'0010;
}

bool PPU::isBackgroundEnabled() const noexcept { // bit0
  return LCDC & 0b0000'0001;
}

std::size_t PPU::screen_y() const noexcept {
  return SCY;
}

std::size_t PPU::screen_x() const noexcept {
  return SCX;
}

byte PPU::currentScanline() const noexcept {
  return LY;
}

void PPU::updateScanline() noexcept {
  ++LY;
}

void PPU::resetScanline() noexcept {
  LY = 0;
}

void PPU::compareScanline() noexcept {
  if(LYC == LY) intr.request(Interrupt::kind::vblank);
}

std::array<PPU::palette_index, 4> PPU::bgp() const noexcept {
  const palette_index pal_0 = BGP & 0b0000'0011;
  const palette_index pal_1 = (BGP & 0b0000'1100) >> 2;
  const palette_index pal_2 = (BGP & 0b0011'0000) >> 4;
  const palette_index pal_3 = (BGP & 0b1100'0000) >> 6;

  return {pal_0, pal_1, pal_2, pal_3};
}

std::array<PPU::palette_index, 4> PPU::obp0() const noexcept {
  const palette_index pal_0 = OBP0 & 0b0000'0011;
  const palette_index pal_1 = (OBP0 & 0b0000'1100) >> 2;
  const palette_index pal_2 = (OBP0 & 0b0011'0000) >> 4;
  const palette_index pal_3 = (OBP0 & 0b1100'0000) >> 6;

  return {pal_0, pal_1, pal_2, pal_3};
}

std::array<PPU::palette_index, 4> PPU::obp1() const noexcept {
  const palette_index pal_0 = OBP1 & 0b0000'0011;
  const palette_index pal_1 = (OBP1 & 0b0000'1100) >> 2;
  const palette_index pal_2 = (OBP1 & 0b0011'0000) >> 4;
  const palette_index pal_3 = (OBP1 & 0b1100'0000) >> 6;

  return {pal_0, pal_1, pal_2, pal_3};
}

std::size_t PPU::window_y() const noexcept {
  return WY;
}

std::size_t PPU::window_x() const noexcept {
  return WX;
}

PPU::PPU(Interrupt &intr, IO &io) noexcept :
    intr{intr},
    LCDC{io.LCDC},
    STAT{io.STAT},
    SCY{io.SCY},
    SCX{io.SCX},
    LY{io.LY},
    LYC{io.LYC},
    BGP{io.BGP},
    OBP0{io.OBP0},
    OBP1{io.OBP1},
    WY{io.WY},
    WX{io.WX} {}

void PPU::update(const std::size_t cycles) noexcept {
  // Implement this
}

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

byte &PPU::operator[](const std::size_t index) noexcept {
  if(index < vram_end) return m_vram[index - vram];
  else if(index < oam_end) return m_oam[index - oam];
  else assert(false);
}

void PPU::reset() noexcept {
  m_vram.fill(byte{});
  m_oam.fill(byte{});
  m_screen.fill(scanline_t{});
}

}
