#pragma once

#include <cstddef>
// clang-format off

namespace LR35902 {

// GameBoy memory layout                    [half closed range)
//                                          address |  size  |   located in
// -------------------------------------------------------------------------
inline constexpr std::size_t rst_00 =        0x0000; // reset vectors
inline constexpr std::size_t rst_08 =        0x0008;
inline constexpr std::size_t rst_10 =        0x0010;
inline constexpr std::size_t rst_18 =        0x0018;
inline constexpr std::size_t rst_20 =        0x0020;
inline constexpr std::size_t rst_28 =        0x0028;
inline constexpr std::size_t rst_30 =        0x0030;
inline constexpr std::size_t rst_38 =        0x0038;

inline constexpr std::size_t vblank =        0x0040; // interrupt vectors
inline constexpr std::size_t lcd_stat =      0x0048;
inline constexpr std::size_t timer =         0x0050;
inline constexpr std::size_t serial =        0x0058;
inline constexpr std::size_t joypad =        0x0060;

inline constexpr std::size_t header_start =  0x0100;
inline constexpr std::size_t header_end =    0x0150;
                                                     
inline constexpr std::size_t rom0 =          0x0000; // 32KB      Cartridge
inline constexpr std::size_t rom0_end =      0x4000;

inline constexpr std::size_t romx =          0x4000;
inline constexpr std::size_t romx_end =      0x8000;

inline constexpr std::size_t vram =          0x8000; // 8KB       PPU
inline constexpr std::size_t vram_end =      0xa000;

inline constexpr std::size_t sram =          0xa000; // 8KB       Cartridge
inline constexpr std::size_t sram_end =      0xc000;

inline constexpr std::size_t wram0 =         0xc000; // 8KB       Built-in
inline constexpr std::size_t wram0_end =     0xd000;

inline constexpr std::size_t wramx =         0xd000;
inline constexpr std::size_t wramx_end =     0xe000;

inline constexpr std::size_t echo =          0xe000; // 7KB+512B  Built-in
inline constexpr std::size_t echo_end =      0xfe00;

inline constexpr std::size_t oam =           0xfe00; // 160B      PPU
inline constexpr std::size_t oam_end =       0xfea0;

inline constexpr std::size_t noUsable =      0xfea0; // 96B       Built-in
inline constexpr std::size_t noUsable_end =  0xff00;

inline constexpr std::size_t io =            0xff00; // 128B      IO
inline constexpr std::size_t io_end =        0xff80;

inline constexpr std::size_t hram =          0xff80; // 127B      Built-in
inline constexpr std::size_t hram_end =      0xffff;

inline constexpr std::size_t IE =            0xffff; // 1B        Interrupts
// -------------------------------------------------------------------------
                                             // Total:  64KB
}

