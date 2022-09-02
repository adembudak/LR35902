#include <LR35902/config.h>
#include <LR35902/interrupt/interrupt.h>
#include <LR35902/io/io.h>
#include <LR35902/memory_map.h>
#include <LR35902/ppu/ppu.h>

#include <cassert>
#include <cstddef>

namespace LR35902 {

// LCDC register related members
bool PPU::isLCDEnabled() const noexcept { // bit7
  return LCDC & 0b1000'0000;
}

std::size_t PPU::windowTilemapBaseAddress() const noexcept { // bit6
  return (LCDC & 0b0100'0000) ? 0x1C00 : 0x1800;
}

// REVISIT: this effected by bit0
bool PPU::isWindowEnabled() const noexcept { // bit5
  return LCDC & 0b0010'0000;
}

std::size_t PPU::backgroundTilesetBaseAddress() const noexcept { // bit4
  return (LCDC & 0b0001'0000) ? 0x0800 : 0x0000;
}

// window and background share same space, so this does same thing above
std::size_t PPU::windowTilesetBaseAddress() const noexcept { // bit4
  return (LCDC & 0b0001'0000) ? 0x0800 : 0x0000;
}

std::size_t PPU::backgroundTilemapBaseAddress() const noexcept { // bit3
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

// STAT register related members
enum class PPU::state : std::uint8_t {
  searching_oam = 0b10,
  drawing = 0b11,
  hblanking = 0b00,
  vblanking = 0b01
};

enum class PPU::source : std::uint8_t { hblank, vblank, oam, coincidence };

PPU::state PPU::mode() const noexcept { // bit0, bit1
  switch(STAT & 0b0000'0011) {
  case 0b00: return state::hblanking;
  case 0b01: return state::vblanking;
  case 0b10: return state::searching_oam;
  case 0b11: return state::drawing;
  }
}

// clang-format off
void PPU::mode(const state s) noexcept { // bit0, bit1
  switch(s) {
  case state::hblanking:      STAT &= 0b1111'1100;          break;
  case state::vblanking:     (STAT &= 0b1111'1100) |= 0b01; break;
  case state::searching_oam: (STAT &= 0b1111'1100) |= 0b10; break;
  case state::drawing:        STAT |= 0b0000'0011;          break;
  }
}

void PPU::coincidence(const bool b) noexcept {
  if(b) STAT |= 0b0000'0100;
  else  STAT &= 0b1111'1011;
}

bool PPU::interruptSource(const source s) const noexcept {
  switch(s) {
  case source::hblank:      return STAT & 0b0000'1000; // bit 3
  case source::vblank:      return STAT & 0b0001'0000; // bit 4
  case source::oam:         return STAT & 0b0010'0000; // bit 5
  case source::coincidence: return STAT & 0b0100'0000; // bit 6
  }
}
// clang-format on

// SCY/SCX registers related members
std::size_t PPU::viewport_y() const noexcept {
  return SCY % viewport_h;
}

std::size_t PPU::viewport_x() const noexcept {
  return SCX % viewport_w;
}

// LY/LYC registers related members
byte PPU::currentScanline() const noexcept {
  return LY;
}

void PPU::updateScanline() noexcept {
  ++LY;
}

void PPU::resetScanline() noexcept {
  LY = 0;
}

bool PPU::checkCoincidence() const noexcept {
  return LYC == LY;
}

// BGP/OBP0/OBP1 palette registers related members
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

// WY/WX palette registers related members
std::size_t PPU::window_y() const noexcept {
  return WY;
}

std::size_t PPU::window_x() const noexcept {
  return WX - 7;
}

void PPU::fetchBackground() noexcept {

  for(std::size_t tile_nth = 0; tile_nth < max_tile_screen_x; ++tile_nth) {
    const std::size_t tile_index = ((currentScanline() / tile_h) * max_tile_screen_x) + tile_nth;

    const std::size_t currently_scanning_tileline = currentScanline() % tile_h;

    const std::size_t tile_attribute = readVRAM(backgroundTilemapBaseAddress() + tile_index);

    const std::size_t tileline_address = backgroundTilesetBaseAddress() + (tile_attribute * tile_size) +
                                         (currently_scanning_tileline * tileline_size);

    const byte tileline_upper = readVRAM(tileline_address);
    const byte tileline_lower = readVRAM(tileline_address + 1uz);

    std::uint8_t mask = 0b1000'0000;
    for(std::size_t i = 0; i < tile_w; ++i, mask >>= 1) {
      const std::size_t y = currentScanline();
      const std::size_t x = ((tile_index % max_tile_screen_x) * tile_w) + i;

      const bool lo = bool(tileline_upper & mask); // the 2 bits in the
      const bool hi = bool(tileline_lower & mask); // 2bpp format
      const palette_index pi = (hi << 1) | lo;

      m_screen[y][x] = bgp()[pi];
    }
  }
}

void PPU::fetchWindow() noexcept {
  if(currentScanline() < window_y()) return;

  for(std::size_t tile_nth = 0; tile_nth < max_tile_screen_x; ++tile_nth) {
    const std::size_t tile_index = ((currentScanline() / tile_h) * max_tile_screen_x) + tile_nth;

    const std::size_t currently_scanning_tileline = currentScanline() % tile_h;

    const std::size_t tile_attribute = readVRAM(windowTilemapBaseAddress() + tile_index);

    const std::size_t tileline_address = windowTilesetBaseAddress() + (tile_attribute * tile_size) +
                                         (currently_scanning_tileline * tileline_size);

    const byte tileline_upper = readVRAM(tileline_address);
    const byte tileline_lower = readVRAM(tileline_address + 1uz);

    std::uint8_t mask = 0b1000'0000;
    for(std::size_t i = 0; i < tile_w; ++i, mask >>= 1) {
      const std::size_t x = ((tile_index % max_tile_screen_x) * tile_w) + i;
      if(x < window_x()) continue;
      const std::size_t y = currentScanline();

      const bool lo = bool(tileline_upper & mask);
      const bool hi = bool(tileline_lower & mask);
      const palette_index pi = (hi << 1) | lo;

      m_screen[y][x] = bgp()[pi];
    }
  }
}

/*
  // REVISIT: handle x pos. priority, travserse oam in reverse way
  // REVISIT: implement this function with ranges, something like:
  namespace rg = ranges;
  namespace rv = rg::views;
  namespace ra = rg::actions;

  auto rng =  m_oam
             | rv::chunk(4)
             | rg::to<vector>
             | ra::drop_while([&](auto &a) { return a[0] < y || a[0] >= y_end; })
             | ra::sort([](auto &a, auto &b) { return a[1] < b[1]; })
             | ra::reverse
             | ra::take(10);
   */

constexpr std::uint8_t max_possible_sprite_on_scanline = 10;

void PPU::fetchSprites() noexcept {
  std::uint8_t total_sprites_on_scanline = 0;

  for(std::size_t i = 0; i < std::size(m_oam); i += 4uz) {
    const byte y = readOAM(i + 0uz) - tile_size; // height occupied by a sprite: [y, y_end)
    const byte y_end = y + (isBigSprite() ? 2 * tile_h : tile_h);

    if(currentScanline() < y) continue;
    if(currentScanline() >= y_end) continue;
    if(++total_sprites_on_scanline > max_possible_sprite_on_scanline) break;

    const byte x = readOAM(i + 1uz) - tile_w;
    const byte tile_index = readOAM(i + 2uz);

    const byte atrb = readOAM(i + 3uz);
    const bool bgHasPriority = atrb & 0b1000'0000; // REVISIT: handle bgHasPriority
    const bool yflip = atrb & 0b0100'0000;
    const bool xflip = atrb & 0b0010'0000;
    const std::size_t palette = bool(atrb & 0b0001'0000); // OBP0 or OBP1?

    const std::size_t currently_scanning_tileline = currentScanline() - y;

    const std::size_t currently_scanning_tileline_position = [&]() {
      if(yflip)
        if(isBigSprite()) //
          return 2 * tile_h - 1uz - currently_scanning_tileline;
        else //
          return tile_h - 1uz - currently_scanning_tileline;
      else //
        return currently_scanning_tileline;
    }();

    const std::size_t tile_address =
        (tile_index * tile_index) + (currently_scanning_tileline_position * tileline_size);

    const byte tileline_upper = readVRAM(tile_address);
    const byte tileline_lower = readVRAM(tile_address + 1uz);

    std::uint8_t mask = xflip ? 0b0000'0001 : 0b1000'0000;
    for(std::size_t i = 0; i < tile_w; ++i) {

      const bool lo = bool(tileline_upper & mask);
      const bool hi = bool(tileline_lower & mask);
      const palette_index pi = (hi << 1) | lo;
      if(pi == 0b00) continue; // transparent color

      m_screen[y][x + i] = palette == 0 ? obp0()[pi] : obp1()[pi];

      xflip ? mask <<= 1 : mask >>= 1;
    }
  }
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

/*
+----------------+      +----------------+      +----------------+
| searching_oam  |----->|   drawing      |----->|   hblanking    |
+----------------+      +----------------+      +----------------+
      ^                                                 | ++LY
      |                                                 v
      |-<--------------------------------------<---No---| LY == 144
      |                                                Yes
LY = 0|           +-->---------------------->-+         |
      |           No                          |         |
      | LY == 154 |     +----------------+    |         |
      ^-------<-Yes--<--|   vblanking    |<---v---------v
                        +----------------+
*/
constexpr std::size_t oam_search_period = 20; // [0, 20)
constexpr std::size_t draw_period = 43;       // [20, 63)
constexpr std::size_t hblank_period = 51;     // [63,114)

constexpr std::size_t scanline_period = oam_search_period + draw_period + hblank_period;
static_assert(scanline_period == 114);

constexpr std::size_t vblank_height = 10;
constexpr std::size_t vblank_period = vblank_height * scanline_period;
static_assert(vblank_period == 1140);

constexpr std::size_t vblank_start = 144;
constexpr std::size_t vblank_end = vblank_start + vblank_height; // when LY is in [144, 154) vblanking

std::size_t oam_search_period_counter{};
std::size_t draw_period_counter{};
std::size_t hblank_period_counter{};

std::size_t vblank_period_counter{};
std::size_t scanline_period_counter{};

void PPU::update(const std::size_t cycles) noexcept {
  if(isLCDEnabled()) {
    switch(mode()) {
    case state::searching_oam:
      oam_search_period_counter += cycles;

      if(oam_search_period_counter > oam_search_period) {
        if(isBackgroundEnabled()) fetchBackground();
        if(isWindowEnabled()) fetchWindow();
        if(isSpritesEnabled()) fetchSprites();

        draw_period_counter += (oam_search_period_counter - oam_search_period);
        oam_search_period_counter = 0;
        mode(state::drawing);
      }
      break;

    case state::drawing:
      draw_period_counter += cycles;

      if(draw_period_counter > draw_period) {
        hblank_period_counter += (draw_period_counter - draw_period);
        draw_period_counter = 0;
        mode(state::hblanking);
        if(interruptSource(source::hblank)) intr.request(Interrupt::kind::lcd_stat);
      }
      break;

    case state::hblanking:
      hblank_period_counter += cycles;

      if(hblank_period_counter > hblank_period) {
        const std::size_t left_over_cycles = hblank_period_counter - hblank_period;

        updateScanline();
        coincidence(checkCoincidence());
        if(checkCoincidence() && interruptSource(source::coincidence))
          intr.request(Interrupt::kind::lcd_stat);

        if(currentScanline() == vblank_start) {
          vblank_period_counter += left_over_cycles;

          mode(state::vblanking);
          intr.request(Interrupt::kind::vblank);
        } else {
          oam_search_period_counter += left_over_cycles;

          mode(state::searching_oam);
          if(interruptSource(source::oam)) intr.request(Interrupt::kind::lcd_stat);
        }

        hblank_period_counter = 0;
      }
      break;

    case state::vblanking:
      vblank_period_counter += cycles;
      scanline_period_counter += cycles;

      if(scanline_period_counter > scanline_period) {
        scanline_period_counter -= scanline_period;
        updateScanline();
      }

      if(vblank_period_counter > vblank_period) {
        resetScanline();

        mode(state::searching_oam);
        if(interruptSource(source::oam)) intr.request(Interrupt::kind::lcd_stat);

        oam_search_period_counter += (vblank_period_counter - vblank_period);
        vblank_period_counter = 0;
        scanline_period_counter = 0;
      }
      break;
    }
  }

  else { // LDC is off
    resetScanline();
    mode(state::hblanking);
  }
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
