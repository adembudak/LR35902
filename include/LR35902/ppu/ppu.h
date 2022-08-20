#pragma once

#include <LR35902/config.h>

#include <array>

namespace LR35902 {

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
//  |                   |~~tile [-128,0)~~~~~~tile [0,128)~~~~~~|          |          |                
//  [-----------------------tile set---------------------------)[-------tile map------)
//  [------------------)[------------------)[------------------)[---------)[----------)                
//  0x8000              0x8800              0x9000              0x9800     0x9C00     0xA000           
//                                                                                                     
//
//  tile structure, based on https://www.huderlem.com/demos/gameboy2bpp.html
//
//  VRAM[] = {0xff, 0x00, 0x7e, 0xff, 0x85, 0x81, 0x89, 0x83, 0x93, 0x85, 0xa5, 0x8b, 0xc9, 0x97, 0x7e, 0xff, ... }
//
//  palette[] = {██, ▓▓, ▒▒, ░░}, background and window tiles uses BGP register for palette, objects (sprites) uses OBP0/OBP1.
//  For objects, palette[0] is called *transparent* and in effect, it's not drawn on screen.
//
//  2 bit per pixel
//  each tile is  8 x 8 pixel
//              = 8 x 8 x 2 bit
//              = 128 bit
//              = 16 byte
//
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

struct Interrupt;
class IO;

class PPU {
public:
  static constexpr std::size_t screen_w = 256; // in px
  static constexpr std::size_t screen_h = 256;

  static constexpr std::size_t display_w = 160; // in px
  static constexpr std::size_t display_h = 144;

  static constexpr std::size_t tilemap_size = 6_KiB;
  static constexpr std::size_t tileset_size = 2_KiB;

  static constexpr std::size_t tile_w = 8; // in px
  static constexpr std::size_t tile_h = 8;

  static constexpr std::size_t tileline_size = 2_B;
  static constexpr std::size_t tile_size = tileline_size * tile_h;

  static constexpr std::size_t oam_scan_period_cycles = 80 / 4;
  static constexpr std::size_t drawing_period_cycles = 172 / 4;
  static constexpr std::size_t hblank_period_cycles = 204 / 4;
  static constexpr std::size_t one_scanline_cycles =
      oam_scan_period_cycles + drawing_period_cycles + hblank_period_cycles;
  static constexpr std::size_t vblank_period_cycles = 10 * one_scanline_cycles;

public:
  using palette_index = uint8_t;
  using scanline_t = std::array<palette_index, screen_w>;
  using screen_t = std::array<scanline_t, screen_h>;

private:
  std::array<byte, 8_KiB> m_vram{};
  std::array<byte, 160_B> m_oam{};

private:
  Interrupt &intr;

private:
  // lcd controller
  byte &LCDC;
  [[nodiscard]] bool isLCDEnabled() const noexcept;
  [[nodiscard]] std::size_t windowTileMapIndex() const noexcept;
  [[nodiscard]] bool isWindowEnabled() const noexcept;
  [[nodiscard]] std::size_t backgroundTilesetIndex() const noexcept;
  [[nodiscard]] std::size_t windowTilesetIndex() const noexcept;
  [[nodiscard]] std::size_t backgroundTilemapIndex() const noexcept;
  [[nodiscard]] bool isBigSprite() const noexcept;
  [[nodiscard]] bool isSpritesEnabled() const noexcept;
  [[nodiscard]] bool isBackgroundEnabled() const noexcept;

  // lcd status
  byte &STAT;

  // screen (viewport) y, x
  byte &SCY;
  byte &SCX;

  /// currently drawing scanline
  byte &LY;
  byte &LYC;

  /// palettes
  byte &BGP;
  byte &OBP0;
  byte &OBP1;

  /// window y, x
  byte &WY;
  byte &WX;

private:
  screen_t m_screen;

  void fetchBackground() noexcept;
  void fetchWindow() noexcept;
  void fetchSprites() noexcept;

public:
  PPU(Interrupt &intr, IO &io) noexcept;

  void update(const std::size_t cycles) noexcept;

  [[nodiscard]] byte readVRAM(const std::size_t index) const noexcept;
  void writeVRAM(const std::size_t index, const byte b) noexcept;

  [[nodiscard]] byte readOAM(const std::size_t index) const noexcept;
  void writeOAM(const std::size_t index, const byte b) noexcept;

  [[nodiscard]] byte &operator[](const std::size_t index) noexcept;

  void reset() noexcept;

  friend class DebugView;
};

static_assert(PPU::oam_scan_period_cycles == 20);
static_assert(PPU::drawing_period_cycles == 43);
static_assert(PPU::hblank_period_cycles == 51);
static_assert(PPU::one_scanline_cycles == 114);
static_assert(PPU::vblank_period_cycles == 1140);

}
