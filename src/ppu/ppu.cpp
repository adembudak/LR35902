#include <LR35902/config.h>
#include <LR35902/interrupt/interrupt.h>
#include <LR35902/io/io.h>
#include <LR35902/memory_map.h>
#include <LR35902/ppu/ppu.h>

#include <range/v3/action/reverse.hpp>
#include <range/v3/action/sort.hpp>
#include <range/v3/action/take.hpp>
#include <range/v3/to_container.hpp>
#include <range/v3/view/chunk.hpp>
#include <range/v3/view/counted.hpp>
#include <range/v3/view/iota.hpp>
#include <range/v3/view/remove_if.hpp>

#include <cstddef>
#include <functional>

namespace LR35902 {

namespace rg = ranges;
namespace rv = rg::views;
namespace ra = rg::actions;

constexpr std::size_t tile_w = 8; // in px
constexpr std::size_t tile_h = 8;

constexpr std::size_t max_tiles_on_screen_x = 32;
constexpr std::size_t max_tiles_on_screen_y = 32;

constexpr std::size_t screen_w = tile_w * max_tiles_on_screen_x; // in px
constexpr std::size_t screen_h = tile_h * max_tiles_on_screen_y;

constexpr std::size_t max_tiles_on_viewport_x = 20;
constexpr std::size_t max_tiles_on_viewport_y = 18;

constexpr std::size_t viewport_w = tile_w * max_tiles_on_viewport_x; // in px
constexpr std::size_t viewport_h = tile_h * max_tiles_on_viewport_y;

constexpr std::size_t max_sprites_on_viewport_x = 10;

constexpr std::size_t tileset_block_size = 4_KiB;
constexpr std::size_t tileset_size = 2 * tileset_block_size - 2_KiB; // -2_KiB because blocks are overlapping
static_assert(tileset_size == 6_KiB);

constexpr std::size_t tilemap_block_size = max_tiles_on_screen_x * max_tiles_on_screen_y * 1_B;
constexpr std::size_t tilemap_size = 2 * tilemap_block_size;
static_assert(tilemap_size == 2_KiB);

static_assert(tileset_size + tilemap_size == 8_KiB /* == VRAM size */);

constexpr std::size_t tileline_size = 2_B;
constexpr std::size_t tile_size = tileline_size * tile_h;

constexpr std::size_t oam_search_period = 20; // [0, 20)
constexpr std::size_t draw_period = 43;       // [20, 63)
constexpr std::size_t hblank_period = 51;     // [63,114)

constexpr std::size_t scanline_period = oam_search_period + draw_period + hblank_period;
static_assert(scanline_period == 114);

constexpr std::size_t vblank_height = 10;
constexpr std::size_t vblank_period = vblank_height * scanline_period;
static_assert(vblank_period == 1140);

constexpr std::size_t vblank_start = 144; // when LY in [144, 154)
constexpr std::size_t vblank_end = vblank_start + vblank_height;

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

byte PPU::readVRAM(const std::size_t index) const noexcept {
  if(isVRAMAccessibleToCPU()) return m_vram[index];
  return 0xff;
}

void PPU::writeVRAM(const std::size_t index, const byte b) noexcept {
  if(isVRAMAccessibleToCPU()) m_vram[index] = b;
}

byte PPU::readOAM(const std::size_t index) const noexcept {
  if(isOAMAccessibleToCPU()) return m_oam[index];
  return 0xff;
}

void PPU::writeOAM(const std::size_t index, const byte b) noexcept {
  if(isOAMAccessibleToCPU()) m_oam[index] = b;
}

enum class PPU::state : std::uint8_t {
  searching_oam = 0b10,
  drawing = 0b11,
  hblanking = 0b00,
  vblanking = 0b01
};

enum class PPU::source : std::uint8_t { hblank, vblank, oam, coincidence };

/*
+----------------+       +-------------+       +--------------+
| searching_oam  |------>|   drawing   |------>|  hblanking   |
+----------------+       +-------------+       +--------------+
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

std::size_t ppu_cycles = 0;
void PPU::update(const std::size_t cycles) noexcept {
  ppu_cycles += cycles;

  if(!isLCDEnabled()) { // LCD is off
    ppu_cycles = 0;
    resetScanline();
    mode(state::vblanking);
    return;
  }

  coincidence(checkCoincidence());
  if(checkCoincidence() && interruptSourceEnabled(source::coincidence))
    intr.request(Interrupt::kind::lcd_stat);

  switch(mode()) {
  case state::searching_oam:
    if(ppu_cycles >= oam_search_period) {
      ppu_cycles -= oam_search_period;

      if(isBackgroundEnabled()) fetchBackground();
      if(isWindowEnabled()) fetchWindow();
      if(isSpritesEnabled()) fetchSprites();

      mode(state::drawing); // 2 -> 3
    }
    break;

  case state::drawing:
    if(ppu_cycles >= draw_period) {
      ppu_cycles -= draw_period;

      mode(state::hblanking); // 3 -> 0

      if(interruptSourceEnabled(source::hblank)) //
        intr.request(Interrupt::kind::lcd_stat);
    }
    break;

  case state::hblanking:
    if(ppu_cycles >= hblank_period) {
      ppu_cycles -= cycles;

      updateScanline();

      if(currentScanline() == vblank_start) {
        mode(state::vblanking); // 0 -> 1
        m_drawCallback(m_screen);

        intr.request(Interrupt::kind::vblank);
      } else {
        mode(state::searching_oam); // 0 -> 2

        if(interruptSourceEnabled(source::oam)) //
          intr.request(Interrupt::kind::lcd_stat);
      }
    }
    break;

  case state::vblanking:
    if(ppu_cycles >= scanline_period) {
      ppu_cycles -= scanline_period;

      updateScanline();

      coincidence(checkCoincidence());
      if(checkCoincidence() && interruptSourceEnabled(source::coincidence))
        intr.request(Interrupt::kind::lcd_stat);

      if(currentScanline() >= vblank_end) {
        resetScanline();

        mode(state::searching_oam); // 1 -> 2

        if(interruptSourceEnabled(source::oam)) //
          intr.request(Interrupt::kind::lcd_stat);
      }
    }
    break;
  }
}

void PPU::setDrawCallback(const std::function<void(const screen_t &framebuffer)> &drawCallback) noexcept {
  m_drawCallback = drawCallback;
}

void PPU::reset() noexcept {
  m_vram.fill(byte{});
  m_oam.fill(byte{});
  m_screen.fill(scanline_t{});
}

// LCDC register related members
bool PPU::isLCDEnabled() const noexcept { // bit7
  return LCDC & 0b1000'0000;
}

std::size_t PPU::windowTilemapBaseAddress() const noexcept { // bit6
  return (LCDC & 0b0100'0000) ? 0x1C00 : 0x1800;
}

bool PPU::isWindowEnabled() const noexcept { // bit5
  if(!isBackgroundEnabled())                 //
    return false;
  return LCDC & 0b0010'0000;
}

std::size_t PPU::backgroundTilesetBaseAddress() const noexcept { // bit4
  return (LCDC & 0b0001'0000) ? 0x0000 : 0x0800;
}

// window and background share the same memory space, so this member does the same thing as above
std::size_t PPU::windowTilesetBaseAddress() const noexcept { // bit4
  return (LCDC & 0b0001'0000) ? 0x0000 : 0x0800;
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

bool PPU::interruptSourceEnabled(const source s) const noexcept {
  switch(s) {
  case source::hblank:      return STAT & 0b0000'1000; // bit 3
  case source::vblank:      return STAT & 0b0001'0000; // bit 4
  case source::oam:         return STAT & 0b0010'0000; // bit 5
  case source::coincidence: return STAT & 0b0100'0000; // bit 6
  }
}
// clang-format on

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
int PPU::window_y() const noexcept {
  return WY;
}

int PPU::window_x() const noexcept {
  return WX - 7;
}

/*
Memory parts accessible to CPU, during scanline rendering:

state         | VRAM  | OAM
--------------+-------+------
hblanking     |   ✔   |  ✔
vblanking     |   ✔   |  ✔
searching_oam |   ✔   |  ✕
drawing       |   ✕   |  ✕
*/

bool PPU::isVRAMAccessibleToCPU() const noexcept {
  return mode() != state::drawing;
}

bool PPU::isOAMAccessibleToCPU() const noexcept {
  return mode() == state::hblanking || mode() == state::vblanking;
}

void PPU::fetchBackground() const noexcept {
  const auto tileset = rv::counted(m_vram.begin() + backgroundTilesetBaseAddress(), tileset_block_size) //
                       | rv::chunk(tile_size);

  const auto tilemap = rv::counted(m_vram.begin() + backgroundTilemapBaseAddress(), tilemap_block_size) //
                       | rv::chunk(max_tiles_on_screen_x);

  for(const std::size_t tile_nth : rv::iota(0uz, max_tiles_on_viewport_x)) {
    const std::size_t row = currentScanline() / tile_h;
    const std::size_t tile_address = tilemap[row][tile_nth];

    const std::size_t currently_scanning_tileline = currentScanline() % tile_h;

    const byte tileline_upper = tileset[tile_address][currently_scanning_tileline * 2];
    const byte tileline_lower = tileset[tile_address][currently_scanning_tileline * 2 + 1];

    std::uint8_t mask = 0b1000'0000;
    for(std::size_t i = 0; i != tile_w; ++i, mask >>= 1) {
      const int x = (tile_nth * tile_w) - SCX + i;
      if(x < 0 || x >= viewport_w) continue;

      const int y = currentScanline() - SCY;
      if(y < 0 || y >= viewport_h) continue;

      const bool lo = tileline_upper & mask; // the 2 bits in the
      const bool hi = tileline_lower & mask; // 2bpp format
      const palette_index pi = (hi << 1) | lo;

      m_screen[y][x] = original[bgp()[pi]];
    }
  }
}

void PPU::fetchWindow() const noexcept {
  if(currentScanline() < window_y()) return;

  const auto tileset = rv::counted(m_vram.begin() + windowTilesetBaseAddress(), tileset_block_size) //
                       | rv::chunk(tile_size);

  const auto tilemap = rv::counted(m_vram.begin() + windowTilemapBaseAddress(), tilemap_block_size) //
                       | rv::chunk(max_tiles_on_screen_x);

  for(const std::size_t tile_nth : rv::iota(0uz, max_tiles_on_viewport_x)) {
    const std::size_t row = currentScanline() / tile_h;
    const std::size_t tile_address = tilemap[row][tile_nth];

    const std::size_t currently_scanning_tileline = currentScanline() % tile_h;

    const byte tileline_upper = tileset[tile_address][currently_scanning_tileline * 2];
    const byte tileline_lower = tileset[tile_address][currently_scanning_tileline * 2 + 1];

    std::uint8_t mask = 0b1000'0000;
    for(std::size_t i = 0; i != tile_w; ++i, mask >>= 1) {
      const std::size_t x = (tile_nth * tile_w) + window_x() + i;
      if(x > viewport_w) continue;

      const std::size_t y = currentScanline();

      const bool lo = tileline_upper & mask;
      const bool hi = tileline_lower & mask;
      const palette_index pi = (hi << 1) | lo;

      m_screen[y][x] = original[bgp()[pi]];
    }
  }
}

void PPU::fetchSprites() const noexcept {
  const auto tile_screen_offset_y = 16uz;
  const auto tile_screen_offset_x = 8uz; // when a sprite is on (8, 16), it appears on top-left

  const auto spriteHeight = [&] { return isBigSprite() ? (2 * tile_h) : tile_h; };

  const auto isSpriteVisible = [&](byte y, byte x) -> bool {
    return x > 0 && x < (viewport_w + tile_screen_offset_x - 1uz) && // x is in [1, 166]
           y > 8 && y < (viewport_h + tile_screen_offset_y - 1uz);   // y is in [8, 158]
  };

  const auto isSpriteOnScanline = [&](const byte y) -> bool {
    const int tile_y_on_screen = y - tile_screen_offset_y;

    return LY >= tile_y_on_screen && LY < (tile_y_on_screen + spriteHeight());
  };

  // clang-format off
  const auto 
  sprites_on_scanline = m_oam
                        | rv::chunk(4) // [y, x, tile_index, atrb] x 40
                        | rv::remove_if([&](const auto &o) { return !isSpriteVisible(o[0], o[1]) || !isSpriteOnScanline(o[0]);})
                        | rg::to<std::vector> 
                        | ra::sort([](const auto &a, const auto &b) { return a[1] < b[1]; })
                        | ra::reverse
                        | ra::take(max_sprites_on_viewport_x);

  for(const auto &obj : sprites_on_scanline) {
    const byte y =          obj[0];
    const byte x =          obj[1];
    const byte tile_index = obj[2];
    const byte atrb       = obj[3];

    const bool bgHasPriority = atrb & 0b1000'0000;
    const bool yflip =         atrb & 0b0100'0000;
    const bool xflip =         atrb & 0b0010'0000;
    const bool palette =       atrb & 0b0001'0000; // OBP1 or OBP0?
    // clang-format on
    const byte y_on_screen = y - tile_screen_offset_y;
    const byte x_on_screen = x - tile_screen_offset_x;

    const std::size_t tile_address = tile_index * tile_size;
    auto sprite = rv::counted(m_vram.begin() + tile_address, 2 * spriteHeight()) | rg::to<std::vector>;
    if(yflip) rg::reverse(sprite);

    const std::size_t currently_scanning_tileline = currentScanline() - y_on_screen;
    const byte tileline_upper = sprite[currently_scanning_tileline * 2];
    const byte tileline_lower = sprite[currently_scanning_tileline * 2 + 1];

    std::uint8_t mask = xflip ? 0b0000'0001 : 0b1000'0000;
    for(std::size_t i = 0; i != tile_w; ++i) {
      const bool lo = tileline_upper & mask;
      const bool hi = tileline_lower & mask;
      xflip ? mask <<= 1 : mask >>= 1;

      const palette_index pi = (hi << 1) | lo;
      if(pi == 0b00) continue; // "transparent" color

      const byte y = y_on_screen + currently_scanning_tileline;
      const byte x = x_on_screen + i;

      m_screen[y][x] = bgHasPriority ? original[bgp()[pi]]
                       : palette     ? original[obp1()[pi]]
                                     : original[obp0()[pi]];
    }
  }
}

}
