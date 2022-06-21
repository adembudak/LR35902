#pragma once

#include <cstddef>
// clang-format off

namespace LR35902 {

// GameBoy memory layout                    [half closed range)
//                                          address |  size  |   located in
// -------------------------------------------------------------------------
inline constexpr std::size_t rom0 =          0x0000; // 32KB      Cartridge
inline constexpr std::size_t rom0_end =      0x4000;

inline constexpr std::size_t romx =          0x4000;
inline constexpr std::size_t romx_end =      0x7FFF;

inline constexpr std::size_t vram =          0x8000; // 8KB       PPU
inline constexpr std::size_t vram_end =      0xA000;

inline constexpr std::size_t sram =          0xA000; // 8KB       Cartridge
inline constexpr std::size_t sram_end =      0xBFFF;

inline constexpr std::size_t wram0 =         0xC000; // 8KB       Built-in
inline constexpr std::size_t wram0_end =     0xD000;

inline constexpr std::size_t wramx =         0xD000;
inline constexpr std::size_t wramx_end =     0xE000;

inline constexpr std::size_t echo =          0xE000; // 7KB+512B  Built-in
inline constexpr std::size_t echo_end =      0xFE00;

inline constexpr std::size_t oam =           0xFE00; // 160B      PPU
inline constexpr std::size_t oam_end =       0xFEA0;

inline constexpr std::size_t noUsable =      0xFEA0; // 96B       Built-in
inline constexpr std::size_t noUsable_end =  0xFF00;

inline constexpr std::size_t io =            0xFF00; // 128B      IO
inline constexpr std::size_t io_end =        0xFF80;

inline constexpr std::size_t hram =          0xFF80; // 127B      Built-in
inline constexpr std::size_t hram_end =      0xFFFF;

inline constexpr std::size_t IE =            0xFFFF; // 1B        Interrupts
// -------------------------------------------------------------------------
                                             // Total:  64KB
}

