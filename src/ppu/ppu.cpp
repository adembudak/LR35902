#include <LR35902/config.h>
#include <LR35902/interrupt/interrupt.h>
#include <LR35902/io/io.h>
#include <LR35902/memory_map.h>
#include <LR35902/ppu/ppu.h>

#include <range/v3/action/reverse.hpp>
#include <range/v3/action/sort.hpp>
#include <range/v3/action/take.hpp>
#include <range/v3/action/transform.hpp>
#include <range/v3/algorithm/copy_n.hpp>
#include <range/v3/algorithm/fill.hpp>
#include <range/v3/algorithm/rotate.hpp>
#include <range/v3/functional/comparisons.hpp>
#include <range/v3/range/conversion.hpp>
#include <range/v3/view/chunk.hpp>
#include <range/v3/view/const.hpp>
#include <range/v3/view/counted.hpp>
#include <range/v3/view/filter.hpp>
#include <range/v3/view/iota.hpp>
#include <range/v3/view/join.hpp>
#include <range/v3/view/remove_if.hpp>
#include <range/v3/view/reverse.hpp>
#include <range/v3/view/transform.hpp>

#include <mpark/patterns/match.hpp>

#include <cstddef>
#include <vector>

#ifdef __clang__
  #pragma GCC diagnostic push
  #pragma GCC diagnostic ignored "-Wreturn-type"
#endif

namespace LR35902 {

namespace rg = ranges;
namespace rv = rg::views;
namespace ra = rg::actions;
namespace mp = mpark::patterns;

PPU::PPU(Interrupt &intr, IO &io) noexcept :
    intr{intr},
    io{io} {}

byte PPU::readVRAM(address_t index) const noexcept {
  index = normalize_index(index, mmap::vram);
  if(isVRAMAccessibleToCPU()) return m_vram[index];
  return 0xff;
}

void PPU::writeVRAM(address_t index, const byte b) noexcept {
  is_vram_changed = true;
  index = normalize_index(index, mmap::vram);
  if(isVRAMAccessibleToCPU()) m_vram[index] = b;
}

byte PPU::readOAM(address_t index) const noexcept {
  index = normalize_index(index, mmap::oam);
  if(isOAMAccessibleToCPU()) return m_oam[index];
  return 0xff;
}

void PPU::writeOAM(address_t index, const byte b) noexcept {
  is_oam_changed = true;
  index = normalize_index(index, mmap::oam);
  if(isOAMAccessibleToCPU()) m_oam[index] = b;
}

enum class PPU::source : std::uint8_t { hblank, vblank, oam, coincidence };

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

/*
 * scanline rendering, see: https://youtu.be/3BJU2drrtCM?t=124

                                 total 114 cycles
             +-------------------------------------------------------+
                 20 cycles           43 cycles            51 cycles
             +--------------+     +-------------+     +--------------+
             0-------------19     20-----------62     63-----------113
   LY = 0    |  searching   |---->|   drawing   |---->|  hblanking   |  ++LY
             +--------------+     +-------------+     +--------------+
             ^                                                       v
             |-<-----------------------------No, Scan a line again---| LY == 144?
             |                                                      Yes, Entering VBlank
   LY = 144  +--------------------------->---------------------------v   ++LY
   LY = 145  L-->------------------------>---------------------------v   ++LY
   LY = 146  L-->------------------------>---------------------------v   ++LY
   LY = 147  L-->------------------------>---------------------------v   ++LY
   LY = 148  L-->------------------------>---------------------------v   ++LY
   LY = 149  L-->------------------------>---------------------------v   ++LY
   LY = 150  L-->------------------------>---------------------------v   ++LY
   LY = 151  L-->------------------------>---------------------------v   ++LY
   LY = 152  L-->------------------------>---------------------------v   ++LY
   LY = 153  L-->------------------------>---------------------------+   ++LY
   LY = 0
*/

static std::size_t ppu_cycles = 0;
void PPU::update(const std::size_t cycles) noexcept {
  ppu_cycles += cycles;

  if(!isLCDEnabled()) { // LCD is off
    ppu_cycles = 0;
    resetScanline();
    return;
  }

  coincidence(checkCoincidence());
  if(checkCoincidence() && interruptSourceEnabled(source::coincidence)) intr.request(Interrupt::kind::lcd_stat);

  switch(mode()) {
  case state::searching:
    if(ppu_cycles >= oam_search_period) {
      ppu_cycles %= oam_search_period;

      if(isBackgroundEnabled()) fetchBackground();
      if(isWindowEnabled()) fetchWindow();
      if(isSpritesEnabled()) fetchSprites();

      mode(state::drawing);
    }
    break;

  case state::drawing:
    if(ppu_cycles >= draw_period) {
      ppu_cycles %= draw_period;

      mode(state::hblanking);

      if(interruptSourceEnabled(source::hblank)) //
        intr.request(Interrupt::kind::lcd_stat);
    }
    break;

  case state::hblanking:
    if(ppu_cycles >= hblank_period) {
      ppu_cycles %= hblank_period;

      updateScanline();

      if(currentScanline() == vblank_start) {
        mode(state::vblanking);

        intr.request(Interrupt::kind::vblank);
      } else {
        mode(state::searching);

        if(interruptSourceEnabled(source::oam)) //
          intr.request(Interrupt::kind::lcd_stat);
      }
    }
    break;

  case state::vblanking:
    if(ppu_cycles >= scanline_period) {
      ppu_cycles %= scanline_period;
      updateScanline();

      coincidence(checkCoincidence());
      if(checkCoincidence() && interruptSourceEnabled(source::coincidence)) intr.request(Interrupt::kind::lcd_stat);

      if(currentScanline() >= vblank_end) {
        resetScanline();

        mode(state::searching);

        if(interruptSourceEnabled(source::oam)) //
          intr.request(Interrupt::kind::lcd_stat);
      }
    }
    break;
  }
}

auto PPU::getFrameBuffer() noexcept -> const framebuffer_t & {
  return m_framebuffer;
}

#if defined(WITH_DEBUGGER)

auto PPU::getBackgroundFrame() noexcept -> const framebuffer_t & {
  return m_background_framebuffer;
}

auto PPU::getWindowFrame() noexcept -> const framebuffer_t & {
  return m_window_framebuffer;
}

auto PPU::getSpritesFrame() noexcept -> const framebuffer_t & {
  return m_sprites_framebuffer;
}

#endif

void PPU::reset() noexcept {
  rg::fill(m_vram, byte{});
  rg::fill(m_oam, byte{});
  rg::fill(m_framebuffer, palette_index_t{});

#if defined(WITH_DEBUGGER)
  rg::fill(m_background_framebuffer, palette_index_t{});
  rg::fill(m_window_framebuffer, palette_index_t{});
  rg::fill(m_sprites_framebuffer, palette_index_t{});
#endif
}

PPU::state PPU::mode() const noexcept { // bit0, bit1
  using namespace mp;

  return match(io.STAT & 0b0000'0011)(
      pattern(0b00) = [] { return state::hblanking; }, //
      pattern(0b01) = [] { return state::vblanking; }, //
      pattern(0b10) = [] { return state::searching; }, //
      pattern(0b11) = [] { return state::drawing; });
}

// LCDC register related members
bool PPU::isLCDEnabled() const noexcept { // bit7
  return io.LCDC & 0b1000'0000;
}

std::size_t PPU::windowTilemapBaseAddress() const noexcept { // bit6
  return (io.LCDC & 0b0100'0000) ? 0x1C00 : 0x1800;
}

bool PPU::isWindowEnabled() const noexcept { // bit5
  return io.LCDC & 0b0010'0000;
}

std::size_t PPU::backgroundTilesetBaseAddress() const noexcept { // bit4
  return (io.LCDC & 0b0001'0000) ? 0x0000 : 0x0800;
}

// window and background share the same memory space, so this member does the
// same thing above
std::size_t PPU::windowTilesetBaseAddress() const noexcept { // bit4
  return (io.LCDC & 0b0001'0000) ? 0x0000 : 0x0800;
}

std::size_t PPU::backgroundTilemapBaseAddress() const noexcept { // bit3
  return (io.LCDC & 0b0000'1000) ? 0x1C00 : 0x1800;
}

bool PPU::isBigSprite() const noexcept { // bit2
  return io.LCDC & 0b0000'0100;
}

bool PPU::isSpritesEnabled() const noexcept { // bit1
  return io.LCDC & 0b0000'0010;
}

bool PPU::isBackgroundEnabled() const noexcept { // bit0
  return io.LCDC & 0b0000'0001;
}

// STAT register related members

// clang-format off
void PPU::mode(const state s) noexcept { // bit0, bit1
  using namespace mp;
  match(s) (
    pattern(state::hblanking) = [&] {  io.STAT &= 0b1111'1100;          },
    pattern(state::vblanking) = [&] { (io.STAT &= 0b1111'1100) |= 0b01; },
    pattern(state::searching) = [&] { (io.STAT &= 0b1111'1100) |= 0b10; },
    pattern(state::drawing)   = [&] {  io.STAT |= 0b0000'0011;          }
  );
}

void PPU::coincidence(const bool b) noexcept {
  if(b) io.STAT |= 0b0000'0100;
  else  io.STAT &= 0b1111'1011;
}

bool PPU::interruptSourceEnabled(const source s) const noexcept {
  using namespace mp;
  return match(s) (
    pattern(source::hblank) = [&] { return io.STAT & 0b0000'1000; }, // bit 3
    pattern(source::vblank) = [&] { return io.STAT & 0b0001'0000; }, // bit 4
    pattern(source::oam)    = [&] { return io.STAT & 0b0010'0000; }, // bit 5
    pattern(source::coincidence) = [&] { return io.STAT & 0b0100'0000; }  // bit 6
  );
}
// clang-format on

// LY/LYC registers related members
byte PPU::currentScanline() const noexcept {
  return io.LY;
}

void PPU::updateScanline() noexcept {
  ++io.LY;
}

void PPU::resetScanline() noexcept {
  io.LY = 0;
}

bool PPU::checkCoincidence() const noexcept {
  return io.LYC == io.LY;
}

// BGP/OBP0/OBP1 palette registers related members
std::array<PPU::palette_index_t, 4> PPU::bgp() const noexcept {
  const palette_index_t pal_0 = io.BGP & 0b0000'0011;
  const palette_index_t pal_1 = (io.BGP & 0b0000'1100) >> 2;
  const palette_index_t pal_2 = (io.BGP & 0b0011'0000) >> 4;
  const palette_index_t pal_3 = (io.BGP & 0b1100'0000) >> 6;

  return {pal_0, pal_1, pal_2, pal_3};
}

std::array<PPU::palette_index_t, 4> PPU::obp0() const noexcept {
  const palette_index_t pal_0 = io.OBP0 & 0b0000'0011;
  const palette_index_t pal_1 = (io.OBP0 & 0b0000'1100) >> 2;
  const palette_index_t pal_2 = (io.OBP0 & 0b0011'0000) >> 4;
  const palette_index_t pal_3 = (io.OBP0 & 0b1100'0000) >> 6;

  return {pal_0, pal_1, pal_2, pal_3};
}

std::array<PPU::palette_index_t, 4> PPU::obp1() const noexcept {
  const palette_index_t pal_0 = io.OBP1 & 0b0000'0011;
  const palette_index_t pal_1 = (io.OBP1 & 0b0000'1100) >> 2;
  const palette_index_t pal_2 = (io.OBP1 & 0b0011'0000) >> 4;
  const palette_index_t pal_3 = (io.OBP1 & 0b1100'0000) >> 6;

  return {pal_0, pal_1, pal_2, pal_3};
}

// WY/WX palette registers related members
int PPU::window_y() const noexcept {
  return io.WY;
}

int PPU::window_x() const noexcept {
  return io.WX - 7;
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

std::array<PPU::palette_index_t, PPU::tile_w> PPU::decodeTilelinePaletteIndices(byte tileline_byte_lower,
                                                                                byte tileline_byte_upper) {
  const std::uint16_t hash = (tileline_byte_upper << 8) | tileline_byte_lower;

  if(tileline_cache.contains(hash)) // hit?
    return tileline_cache[hash];

  else {
    std::array<PPU::palette_index_t, PPU::tile_w> temp;
    for(std::uint8_t mask = 0b1000'0000; auto &e : temp) {
      bool bit0 = bool(tileline_byte_lower & mask);
      bool bit1 = bool(tileline_byte_upper & mask);
      e = (bit1 << 1) | bit0;
      mask >>= 1;
    }

    tileline_cache[hash] = temp;
    return temp;
  }
}

void PPU::fetchBackground() {
  if(is_vram_changed) {
    is_vram_changed = false;

    tileset_view = rv::counted(m_vram.begin() + backgroundTilesetBaseAddress(), tileset_block_size) //
                   | rv::chunk(tileline_size)                                                       //
                   | rv::chunk(tile_h);

    tilemap_view = rv::counted(m_vram.begin() + backgroundTilemapBaseAddress(), tilemap_block_size) //
                   | rv::chunk(max_tiles_on_screen_x);
  }

  std::array<palette_index_t, screen_w> buffer;

  const std::size_t dy = (io.SCY + io.LY) % screen_h;
  const std::size_t row = dy / tile_h;
  const std::size_t currently_scannline_tileline = dy % tile_h;

  for(const std::size_t tile_nth : rv::iota(std::size_t{0}, max_tiles_on_screen_x)) {
    const byte index = tilemap_view[row][tile_nth];
    const auto tileline = tileset_view[index][currently_scannline_tileline];
    const auto decoded = decodeTilelinePaletteIndices(tileline[0], tileline[1]);

    for(const std::size_t i : rv::iota(std::size_t{0}, tile_w)) {
      buffer[tile_nth * tile_w + i] = bgp()[decoded[i]];
    }
  }

  rg::rotate(buffer.begin(), buffer.begin() + io.SCX, buffer.end());
  rg::copy_n(buffer.cbegin(), viewport_w, m_framebuffer.begin() + io.LY * viewport_w);
#if defined(WITH_DEBUGGER)
  rg::copy_n(buffer.cbegin(), viewport_w, m_background_framebuffer.begin() + io.LY * viewport_w);
#endif
}

void PPU::fetchWindow() {
  if(currentScanline() < window_y()) return;

  if(is_vram_changed) {
    is_vram_changed = false;

    tileset_view = rv::counted(m_vram.begin() + windowTilesetBaseAddress(), tileset_block_size) //
                   | rv::chunk(tileline_size)                                                   //
                   | rv::chunk(tile_h);                                                         //

    tilemap_view = rv::counted(m_vram.begin() + windowTilemapBaseAddress(), tilemap_block_size) //
                   | rv::chunk(max_tiles_on_screen_x);
  }

  const std::size_t row = currentScanline() / tile_h;
  const std::size_t currently_scanning_tileline = currentScanline() % tile_h;
  const std::size_t window_x_ = (window_x() < 0) ? 0 : window_x();

  if(std::size_t{window_x_ / tile_w} > max_tiles_on_viewport_x) return; // REVISIT: fix what creates this case

  for(const std::size_t tile_nth : rv::iota(std::size_t{window_x_ / tile_w}, max_tiles_on_viewport_x)) {
    const std::size_t tile_index = tilemap_view[row][tile_nth];
    const auto tileline = tileset_view[tile_index][currently_scanning_tileline];
    const auto decoded = decodeTilelinePaletteIndices(tileline[0], tileline[1]);

    for(const std::size_t i : rv::iota(std::size_t{0}, tile_w)) {
      const std::size_t x = (tile_nth * tile_w) + i;
      m_framebuffer[io.LY * viewport_w + x] = bgp()[decoded[i]];
#if defined(WITH_DEBUGGER)
      m_window_framebuffer[io.LY * viewport_w + x] = bgp()[decoded[i]];
#endif
    }
  }
}

/*
                      normal
0xff, 0x00,      ▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓
0x7e, 0xff,      ▒▒░░░░░░░░░░░░▒▒
0x85, 0x81,      ░░████████▓▓██░░
0x89, 0x83,      ░░██████▓▓██▒▒░░
0x93, 0x85,      ░░████▓▓██▒▒▓▓░░
0xa5, 0x8b,      ░░██▓▓██▒▒▓▓▒▒░░
0xc9, 0x97,      ░░▓▓██▒▒▓▓▒▒▒▒░░
0x7e, 0xff       ▒▒░░░░░░░░░░░░▒▒

                   y flipped                          then x flipped
                  (tile lines are reversed)           (bytes are reversed)
                  the tile becomes upside down        e.g:    0x93 == reverseBits(0xc9)
                                                       0b1001'0011 == reverseBits(0b1100'1001)
0x7e, 0xff       ▒▒░░░░░░░░░░░░▒▒   |   0x7e, 0xff,  ▒▒░░░░░░░░░░░░▒▒
0xc9, 0x97,      ░░▓▓██▒▒▓▓▒▒▒▒░░   |   0x93, 0xe9,  ░░▒▒▒▒▓▓▒▒██▓▓░░
0xa5, 0x8b,      ░░██▓▓██▒▒▓▓▒▒░░   |   0xa5, 0xd1,  ░░▒▒▓▓▒▒██▓▓██░░
0x93, 0x85,      ░░████▓▓██▒▒▓▓░░   |   0xc9, 0xa1,  ░░▓▓▒▒██▓▓████░░
0x89, 0x83,      ░░██████▓▓██▒▒░░   |   0x91, 0xc1,  ░░▒▒██▓▓██████░░
0x85, 0x81,      ░░████████▓▓██░░   |   0xa1, 0x81,  ░░██▓▓████████░░
0x7e, 0xff,      ▒▒░░░░░░░░░░░░▒▒   |   0x7e, 0xff,  ▒▒░░░░░░░░░░░░▒▒
0xff, 0x00,      ▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓   |   0xff, 0x00   ▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓
*/

void PPU::fetchSprites() {
  constexpr int sprite_viewport_offset_y = 16;
  constexpr int sprite_viewport_offset_x = 8; // when a sprite is on (8, 16), it appears on top-left

  constexpr int sprite_starts_visible_x = 1;
  constexpr int sprite_ends_visible_x = 168;

  constexpr int sprite_starts_visible_y = 9;
  constexpr int sprite_ends_visible_y = 160;

  const auto spriteHeight = [&] { return isBigSprite() ? double_tile_h : tile_h; };
  const auto numberOfBytesToFetch = [&] { return spriteHeight() * tileline_size; };

  const auto isSpriteOutsideOfTheViewport = [&](const byte x, const byte y) -> bool {
    return x < sprite_starts_visible_x || //
           x >= sprite_ends_visible_x ||  //
           y < sprite_starts_visible_y || //
           y >= sprite_ends_visible_y;
  };

  const auto isSpriteVisibleToScanline = [&](const byte y) -> bool {
    const int viewport_y = y - sprite_viewport_offset_y;

    return io.LY >= viewport_y && io.LY < (viewport_y + spriteHeight());
  };

  // No idea how the lambda body works below...
  // link: https://graphics.stanford.edu/~seander/bithacks.html#ReverseByteWith64BitsDiv
  const auto reverseBits = [](const byte b) { return (b * 0x0202020202ULL & 0x010884422010ULL) % 1023; };

  // clang-format off
  if(is_oam_changed) {
    is_oam_changed = false;
    oam_view = m_oam | rv::chunk(4); // [y, x, tile_index, atrb] x 40
  }

  const auto
  sprites_on_scanline = oam_view
                        | rv::remove_if([&](const auto &o) { return isSpriteOutsideOfTheViewport(o[1], o[0]); })
                        | rv::filter([&](const auto &o) { return isSpriteVisibleToScanline(o[0]); })
                        | rg::to<std::vector>
                        | ra::sort(rg::greater{}, [](const auto& o) { return o[1]; })
                        | ra::take(max_sprites_on_viewport_x);

  for(const auto &obj : sprites_on_scanline) { // scan a line from each tiles
    const byte y     = obj[0];
    const byte x     = obj[1];
    const byte index = obj[2];
    const byte atrb  = obj[3];

    const bool bgHasPriority = atrb & 0b1000'0000;
    const bool yflip         = atrb & 0b0100'0000;
    const bool xflip         = atrb & 0b0010'0000;
    const bool palette       = atrb & 0b0001'0000; // OBP1 or OBP0?
    // clang-format on

    const std::size_t tile_address = index * tile_size;
    auto sprite = rv::counted(m_vram.begin() + tile_address, numberOfBytesToFetch()) | rg::to<std::vector<byte>>;

    if(yflip)
      sprite = sprite                     //
               | rv::const_               //
               | rv::chunk(tileline_size) //
               | rv::reverse              //
               | rv::join                 //
               | rg::to<std::vector<byte>>;

    if(xflip) ra::transform(sprite, reverseBits);

    const int viewport_y = y - sprite_viewport_offset_y;
    const int viewport_x = x - sprite_viewport_offset_x;
    const int currently_scanning_spriteline = currentScanline() - viewport_y;

    const auto spriteLines = sprite | rv::chunk(tileline_size);
    const auto spriteLine_to_scan = spriteLines[currently_scanning_spriteline];
    const auto decoded = decodeTilelinePaletteIndices(spriteLine_to_scan[0], spriteLine_to_scan[1]);

    for(const int i : rv::iota(std::size_t{0}, tile_w)) {
      if(viewport_x + i < 0) continue;
      if(viewport_x + i >= sprite_ends_visible_x) break;

      if(decoded[i] == 0b00) continue; // "transparent" color, palette index 0 is disallowed for sprites (by spec).

      m_framebuffer[io.LY * viewport_w + viewport_x + i] = bgHasPriority ? bgp()[decoded[i]]  //
                                                           : palette     ? obp1()[decoded[i]] //
                                                                         : obp0()[decoded[i]];
#if defined(WITH_DEBUGGER)
      m_sprites_framebuffer[io.LY * viewport_w + viewport_x + i] = bgHasPriority ? bgp()[decoded[i]]  //
                                                                   : palette     ? obp1()[decoded[i]] //
                                                                                 : obp0()[decoded[i]];
#endif
    }
  }
}
} // namespace LR35902

#ifdef __clang__
  #pragma GCC diagnostic pop
#endif
