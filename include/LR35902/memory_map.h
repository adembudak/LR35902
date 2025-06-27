#pragma once

#include <LR35902/config.h>

#include <cstddef>
// clang-format off

namespace LR35902 { namespace mmap {

// GameBoy memory layout                    [half closed range)
//                                          address |  size  |   located in
// -------------------------------------------------------------------------
inline constexpr address_t bootrom_start = 0x0000; // 256B      BootROM
inline constexpr address_t rom0 =          0x0000; // 16KB      Cartridge
  inline constexpr address_t rst_00 =        0x0000; // reset vectors
  inline constexpr address_t rst_08 =        0x0008;
  inline constexpr address_t rst_10 =        0x0010;
  inline constexpr address_t rst_18 =        0x0018;
  inline constexpr address_t rst_20 =        0x0020;
  inline constexpr address_t rst_28 =        0x0028;
  inline constexpr address_t rst_30 =        0x0030;
  inline constexpr address_t rst_38 =        0x0038;

  inline constexpr address_t vblank =        0x0040; // interrupt vectors
  inline constexpr address_t lcd_stat =      0x0048;
  inline constexpr address_t timer =         0x0050;
  inline constexpr address_t serial =        0x0058;
  inline constexpr address_t joypad =        0x0060;
inline constexpr address_t bootrom_end =  0x0100;
  inline constexpr address_t header_start =  0x0100;
    inline constexpr address_t entry_begin =   0x0100; // entry point
    inline constexpr address_t entry_end =     0x0104;

    inline constexpr address_t logo_begin =    0x0104;
    inline constexpr address_t logo_end =      0x0134;

    inline constexpr address_t title_begin =   0x0134;
    inline constexpr address_t title_end =     0x013f;

    inline constexpr address_t mfr_begin =     0x013f; // manufacturer
    inline constexpr address_t mfr_end =       0x0143;

    inline constexpr address_t cgb_support =   0x0143;

    inline constexpr address_t new_licensee0 = 0x0144;
    inline constexpr address_t new_licensee1 = 0x0145;

    inline constexpr address_t sgb_support =   0x0146;

    inline constexpr address_t mbc_code =      0x0147;

    inline constexpr address_t rom_code =      0x0148;
    inline constexpr address_t ram_code =      0x0149;

    inline constexpr address_t destination =   0x014a;
    inline constexpr address_t old_licensee =  0x014b;
    inline constexpr address_t game_version =  0x014c;

    inline constexpr address_t csum_begin =    0x0134; // checksum
    inline constexpr address_t csum_end =      0x014d;
    inline constexpr address_t csum_result =   0x014d;
  inline constexpr address_t header_end =    0x0150;

inline constexpr address_t rom0_end =      0x4000;

inline constexpr address_t romx =          0x4000; // 16KB
inline constexpr address_t romx_end =      0x8000;

inline constexpr address_t vram =          0x8000; // 8KB       PPU
inline constexpr address_t vram_end =      0xa000;

inline constexpr address_t sram =          0xa000; // 8KB       Cartridge
inline constexpr address_t sram_end =      0xc000;

inline constexpr address_t wram0 =         0xc000; // 8KB       Built-in
inline constexpr address_t wram0_end =     0xd000;

inline constexpr address_t wramx =         0xd000;
inline constexpr address_t wramx_end =     0xe000;

inline constexpr address_t echo =          0xe000; // 7KB+512B  Built-in
inline constexpr address_t echo_end =      0xfe00;

inline constexpr address_t oam =           0xfe00; // 160B      PPU
inline constexpr address_t oam_end =       0xfea0;

inline constexpr address_t noUse =         0xfea0; // 96B       Built-in
inline constexpr address_t noUse_end =     0xff00;

inline constexpr address_t io =            0xff00; // 128B      IO
inline constexpr address_t io_end =        0xff80;

inline constexpr address_t hram =          0xff80; // 127B      Built-in
inline constexpr address_t hram_end =      0xffff;

inline constexpr address_t IE =            0xffff; // 1B        Interrupts
// -------------------------------------------------------------------------
                                             // Total:  64KB
}

[[nodiscard]]
inline address_t normalize_index(const address_t index, const address_t mem) {
  return index % mem;
}

inline constexpr address_t sram_bank_size = mmap::sram_end - mmap::sram;
inline constexpr address_t rom_bank_size = mmap::romx_end - mmap::romx;

static_assert(sram_bank_size == 8 * 1024); // 8KiB
static_assert(rom_bank_size == 16 * 1024); // 16 KiB
}

