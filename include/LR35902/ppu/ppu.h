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

class Interrupt;
class IO;

class PPU {
public:
  using palette_index = std::uint8_t;
  using framebuffer_t = std::array<palette_index, 144 * 160 * 1_B>;

public:
  PPU(Interrupt &intr, IO &io) noexcept;

  [[nodiscard]] byte readVRAM(address_t index) const noexcept;
  void writeVRAM(address_t index, const byte b) noexcept;

  [[nodiscard]] byte readOAM(address_t index) const noexcept;
  void writeOAM(address_t index, const byte b) noexcept;

  void update(const std::size_t cycles) noexcept;

  [[nodiscard]] auto getFrameBuffer() noexcept -> const framebuffer_t &;
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
  std::array<palette_index, 4> bgp() const noexcept;
  std::array<palette_index, 4> obp0() const noexcept;
  std::array<palette_index, 4> obp1() const noexcept;

  /// window y, x
  int window_y() const noexcept;
  int window_x() const noexcept;

  bool isVRAMAccessibleToCPU() const noexcept;
  bool isOAMAccessibleToCPU() const noexcept;

  /// drawing
  mutable framebuffer_t m_framebuffer{};

  void fetchBackground() const;
  void fetchWindow() const;
  void fetchSprites() const;

  friend class DMA;
  friend class DebugView;
};
}
