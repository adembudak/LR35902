#pragma once

#include <LR35902/config.h>

#include <range/v3/view/chunk.hpp>
#include <range/v3/view/subrange.hpp>

#include <array>

namespace LR35902 {
namespace rg = ranges;

class Interrupt;
class IO;

// clang-format off
//
//  VRAM
//  |                  (each tile is 16 bytes)                  | (1KB = 32x32 = [0,1024) indexes)
//  |  2KB = 128 tiles  |  2KB = 128 tiles  |  2KB = 128 tiles  | 1KB      | 1KB      | = 8KB total
//  |                   |                   |                   |          |          |
//  |   Block 0         |    Block 1        |    Block 2        |*LCDC.3=0*|*LCDC.3=1*| (Background)
//  |+++++++++++++++LCDC.4=1++++++++++++++++|                   |          |          |
//  |++tile [0,128)+++++++++tile [128,256)++|                   |,LCDC.6=0,|,LCDC.6=1,| (Window)
//  |                   |~~~~~~~~~~~~~~~~LCDC.4=0~~~~~~~~~~~~~~~|          |          |
//  |                   v~~~tile [0,128)~~~~~~tile [128,256)~~~~|          |          |
//  v-----------------------tile set---------------------------)[-------tile map------)
//  [------------------)[------------------)[------------------)[---------)[----------)
//  0x8000              0x8800              0x9000              0x9800     0x9C00     0xA000
//
//  tile set or tileset: where to get tiles from
//  tile map or tilemap: where to put the tiles
//
//  tile structure, based on https://www.huderlem.com/demos/gameboy2bpp.html
//
//  VRAM[] = {0xff, 0x00, 0x7e, 0xff, 0x85, 0x81, 0x89, 0x83, 0x93, 0x85, 0xa5, 0x8b, 0xc9, 0x97, 0x7e, 0xff, ... }
//
//  palette[] = {██, ▓▓, ▒▒, ░░}, background and window tiles uses BGP register for palette, objects (sprites) uses OBP0/OBP1
//
//  Each tile is 8x8 pixel and 2 bit per pixel format is used:
//              = 8 x 8 x 2 bit
//              = 128 bit
//              = 16 byte
//  a pixel
//    v
//  |lo |
//  |hi | -> specifies palette[(hi<<1) | lo]
//    v
//  | 1 | 1 | 1 | 1 | 1 | 1 | 1 | 1 | // 0xff  | <- 2 byte creates a line of the tile
//  | 0 | 0 | 0 | 0 | 0 | 0 | 0 | 0 | // 0x00  | each pixels of this tileline use palette[0b01] == palette[1]    ▓▓ ▓▓ ▓▓ ▓▓ ▓▓ ▓▓ ▓▓ ▓▓

//  | 0 | 1 | 1 | 1 | 1 | 1 | 1 | 0 | // 0x7e
//  | 1 | 1 | 1 | 1 | 1 | 1 | 1 | 1 | // 0xff  | pal[2], pal[3], pal[3], pal[3], pal[3], pal[3], pal[3], pal[2]  ▒▒ ░░ ░░ ░░ ░░ ░░ ░░ ▒▒

//  | 1 | 0 | 0 | 0 | 0 | 1 | 0 | 1 | // 0x85
//  | 1 | 0 | 0 | 0 | 0 | 0 | 0 | 1 | // 0x81  | pal[3], pal[0], pal[0], pal[0], pal[0], pal[1], pal[0], pal[3]  ░░ ██ ██ ██ ██ ▓▓ ██ ░░       ▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓
//                                                                                                                                             ▒▒░░░░░░░░░░░░▒▒
//  | 1 | 0 | 0 | 0 | 1 | 0 | 0 | 1 | // 0x89                                                                                                  ░░████████▓▓██░░
//  | 1 | 0 | 0 | 0 | 0 | 0 | 1 | 1 | // 0x83  | pal[3], pal[0], pal[0], pal[0], pal[1], pal[0], pal[2], pal[3]  ░░ ██ ██ ██ ▓▓ ██ ▒▒ ░░       ░░██████▓▓██▒▒░░
//                                                                                                                                             ░░████▓▓██▒▒▓▓░░
//  | 1 | 0 | 0 | 1 | 0 | 0 | 1 | 1 | // 0x93                                                                                                  ░░██▓▓██▒▒▓▓▒▒░░
//  | 1 | 0 | 0 | 0 | 0 | 1 | 0 | 1 | // 0x85  | pal[3], pal[0], pal[0]. pal[1], pal[0], pal[2], pal[1], pal[3]  ░░ ██ ██ ▓▓ ██ ▒▒ ▓▓ ░░       ░░▓▓██▒▒▓▓▒▒▒▒░░
//                                                                                                                                             ▒▒░░░░░░░░░░░░▒▒
//  | 1 | 0 | 1 | 0 | 0 | 1 | 0 | 1 | // 0xa5
//  | 1 | 0 | 0 | 0 | 1 | 0 | 1 | 1 | // 0x8b  | pal[3], pal[0], pal[1], pal[0], pal[2], pal[1], pal[2], pal[3]  ░░ ██ ▓▓ ██ ▒▒ ▓▓ ▒▒ ░░

//  | 1 | 1 | 0 | 0 | 1 | 0 | 0 | 1 | // 0xc9
//  | 1 | 0 | 0 | 1 | 0 | 1 | 1 | 1 | // 0x97  | pal[3], pal[1], pal[0], pal[2], pal[1], pal[2], pal[2], pal[3]  ░░ ▓▓ ██ ▒▒ ▓▓ ▒▒ ▒▒ ░░

//  | 0 | 1 | 1 | 1 | 1 | 1 | 1 | 0 | // 0x7e
//  | 1 | 1 | 1 | 1 | 1 | 1 | 1 | 1 | // 0xff  | pal[2], pal[3], pal[3], pal[3], pal[3], pal[3], pal[3], pal[2]  ▒▒ ░░ ░░ ░░ ░░ ░░ ░░ ▒▒

// clang-format on

class PPU {
public:
  static constexpr std::size_t tile_w = 8; // in px
  static constexpr std::size_t tile_h = 8;
  static constexpr std::size_t double_tile_h = 2 * tile_h;

  static constexpr std::size_t max_tiles_on_screen_x = 32;
  static constexpr std::size_t max_tiles_on_screen_y = 32;

  static constexpr std::size_t screen_w = tile_w * max_tiles_on_screen_x; // in px
  static constexpr std::size_t screen_h = tile_h * max_tiles_on_screen_y;
  static_assert(screen_w == 256);
  static_assert(screen_h == 256);

  static constexpr std::size_t max_tiles_on_viewport_x = 20;
  static constexpr std::size_t max_tiles_on_viewport_y = 18;

  static constexpr std::size_t viewport_w = tile_w * max_tiles_on_viewport_x; // in px
  static constexpr std::size_t viewport_h = tile_h * max_tiles_on_viewport_y;
  static_assert(viewport_w == 160);
  static_assert(viewport_h == 144);

  static constexpr std::size_t max_sprites_on_viewport_x = 10;

  static constexpr std::size_t tileset_block_size = 4_KiB;
  static constexpr std::size_t tileset_size = 2 * tileset_block_size - 2_KiB; // -2_KiB because blocks are overlapping
  static_assert(tileset_size == 6_KiB);

  static constexpr std::size_t tilemap_block_size = max_tiles_on_screen_x * max_tiles_on_screen_y * 1_B;
  static constexpr std::size_t tilemap_size = 2 * tilemap_block_size;
  static_assert(tilemap_size == 2_KiB);

  static_assert(tileset_size + tilemap_size == 8_KiB /* == VRAM size */);

  static constexpr std::size_t tileline_size = 2_B;
  static constexpr std::size_t tile_size = tileline_size * tile_h;

  using palette_index_t = std::uint8_t;
  using framebuffer_t = std::array<palette_index_t, viewport_h * viewport_w * 1_B>;
  using tileset_view_t = rg::chunk_view<rg::chunk_view<rg::subrange<byte *, byte *, rg::subrange_kind::sized>>>;
  using tilemap_view_t = rg::chunk_view<rg::subrange<byte *, byte *, rg::subrange_kind::sized>>;
  using oam_view_t = rg::chunk_view<rg::ref_view<std::array<byte, 160_B>>>;

public:
  PPU(Interrupt &intr, IO &io) noexcept;

  [[nodiscard]] byte readVRAM(address_t index) const noexcept;
  void writeVRAM(address_t index, const byte b) noexcept;

  [[nodiscard]] byte readOAM(address_t index) const noexcept;
  void writeOAM(address_t index, const byte b) noexcept;

  void update(const std::size_t cycles) noexcept;

  [[nodiscard]] auto getBackgroundFrame() noexcept -> const framebuffer_t &;
  [[nodiscard]] auto getWindowFrame() noexcept -> const framebuffer_t &;
  [[nodiscard]] auto getSpritesFrame() noexcept -> const framebuffer_t &;
  void reset() noexcept;

  enum class state : std::uint8_t {
    searching = 0b10, // 2
    drawing = 0b11,   // 3
    hblanking = 0b00, // 0
    vblanking = 0b01  // 1
  };

  [[nodiscard]] state mode() const noexcept;

private:
  std::array<byte, 8_KiB> m_vram{};
  std::array<byte, 160_B> m_oam{};

  Interrupt &intr;
  IO &io;

  /// lcd controller
  [[nodiscard]] bool isLCDEnabled() const noexcept;
  [[nodiscard]] std::size_t windowTilemapBaseAddress() const noexcept;
  [[nodiscard]] bool isWindowEnabled() const noexcept;
  [[nodiscard]] std::size_t backgroundTilesetBaseAddress() const noexcept;
  [[nodiscard]] std::size_t windowTilesetBaseAddress() const noexcept;
  [[nodiscard]] std::size_t backgroundTilemapBaseAddress() const noexcept;
  [[nodiscard]] bool isBigSprite() const noexcept;
  [[nodiscard]] bool isSpritesEnabled() const noexcept;
  [[nodiscard]] bool isBackgroundEnabled() const noexcept;

  /// lcd status
  enum class source : std::uint8_t;

  void mode(const state s) noexcept;
  void coincidence(const bool b) noexcept;
  bool interruptSourceEnabled(const source s) const noexcept;

  /// currently drawing scanline
  byte currentScanline() const noexcept;
  void updateScanline() noexcept;
  void resetScanline() noexcept;
  bool checkCoincidence() const noexcept;

  /// palettes
  std::array<palette_index_t, 4> bgp() const noexcept;
  std::array<palette_index_t, 4> obp0() const noexcept;
  std::array<palette_index_t, 4> obp1() const noexcept;

  std::unordered_map<std::uint16_t, std::array<palette_index_t, tile_w>> tileline_cache;

  std::array<palette_index_t, tile_w> decodeTilelinePaletteIndices(byte tileline_byte_lower, byte tileline_byte_upper);

  /// window y, x
  int window_y() const noexcept;
  int window_x() const noexcept;

  bool isVRAMAccessibleToCPU() const noexcept;
  bool isOAMAccessibleToCPU() const noexcept;

  /// drawing
  framebuffer_t m_background_framebuffer{};
  framebuffer_t m_window_framebuffer{};
  framebuffer_t m_sprites_framebuffer{};

  tileset_view_t tileset_view;
  tilemap_view_t tilemap_view;
  oam_view_t oam_view;

  bool is_vram_changed = true;
  bool is_oam_changed = true;

  void fetchBackground();
  void fetchWindow();
  void fetchSprites();

  friend class DMA;
  friend class DebugView;
};
}
