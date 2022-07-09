#pragma once

#include <LR35902/config.h>

#include <array>

namespace LR35902 {

/////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                                                                                     //
//  |                  (each tile is 16 bytes)                  | (1KB = 32x32 = [0,1024) indexes)     //
//  |  2KB = 128 tiles  |  2KB = 128 tiles  |  2KB = 128 tiles  | 1KB      | 1KB      | = 8KB total    //
//  |                   |                   |                   |          |          |                //
//  |   Block 0         |    Block 1        |    Block 2        |*LCDC.3=0*|*LCDC.3=1*| (Background)   //
//  |+++++++++++++++LCDC.4=1++++++++++++++++|                   |          |          |                //
//  |++tile [0,128)+++++++++tile [128,256)++|                   |,LCDC.6=0,|,LCDC.6=1,| (Window)       //
//  |                   |~~~~~~~~~~~~~~~~LCDC.4=0~~~~~~~~~~~~~~~|          |          |                //
//  |                   |~~tile [-128,0)~~~~~~tile [0,128)~~~~~~|          |          |                //
//  [------------------)[------------------)[------------------)[---------)[----------)                //
//  0x8000              0x8800              0x9000              0x9800     0x9C00     0xA000           //
//                                                                                                     //
/////////////////////////////////////////////////////////////////////////////////////////////////////////

class PPU {
  std::array<byte, 8_KiB> m_vram{};
  std::array<byte, 160_B> m_oam{};

public:

  [[nodiscard]] byte readVRAM(const std::size_t index) const noexcept;
  void writeVRAM(const std::size_t index, const byte b) noexcept;

  [[nodiscard]] byte readOAM(const std::size_t index) const noexcept;
  void writeOAM(const std::size_t index, const byte b) noexcept;

  [[nodiscard]] byte &operator[](const std::size_t index) noexcept;

  friend class DebugView;
};

}
