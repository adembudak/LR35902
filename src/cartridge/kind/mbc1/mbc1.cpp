#include <LR35902/cartridge/kind/mbc1/mbc1.h>
#include <LR35902/config.h>
#include <LR35902/memory_map.h>

#include <mpark/patterns/match.hpp>
#include <mpark/patterns/when.hpp>

#include <range/v3/view/chunk.hpp>
#include <range/v3/view/const.hpp>

#include <cstddef>

// Verbatim implementation of:
// https://gbdev.io/pandocs/MBC1.html

/*
MBC1 Registers
---
 The term "register" might be misleading, these are not byte adressed registers,
instead an attempt to write certain ranges of the read only memory, which will be
interpreted as numerical value and will be used as an indicator for addressing behavior.
e.g. a bank number, whether RAM enabled

Register 0:
   7   6   5   4   3   2   1   0
 ┌───┬───┬───┬───┬───┬───┬───┬───┐ Memory range:
 │*x │ x │ x │ x │   │   │   │   │ [0x0000, 0x2000)
 └───┴───┴───┴───┴───┴───┴───┴───┘
 RAM enable register. RAM enabled only when lower nibble of the written byte is 0x0A,
 otherwise disabled. Effectively a boolean.

Register 1:
 ┌───┬───┬───┬───┬───┬───┬───┬───┐
 │ x │ x │ x │   │   │   │   │   │ [0x2000, 0x4000)
 └───┴───┴───┴───┴───┴───┴───┴───┘
 Possible values: [0,32)
 Determines which ROM bank to select:
 if(register_3 == 1) // see Register 3
   mem[(register_1 * 16_KiB) + index]
 else
   mem[((register_2 << 5) | register_1) * 16_KiB) + index]

Register 2:
 ┌───┬───┬───┬───┬───┬───┬───┬───┐
 │ x │ x │ x │ x │ x │ x │   │   │ [0x4000, 0x6000]
 └───┴───┴───┴───┴───┴───┴───┴───┘
 Possible values: {0, 1, 2, 3}
 Bank number for RAM or Upper ROM, depends on Register 3

Register 3:
 ┌───┬───┬───┬───┬───┬───┬───┬───┐
 │ x │ x │ x │ x │ x │ x │ x │   │ [0x6000, 0x8000]
 └───┴───┴───┴───┴───┴───┴───┴───┘
 Possible values:
  0 : Upper ROM banking enabled, RAM banking disabled.
  1 : RAM banking enabled, Upper ROM banking disabled. Only Register 1 will be used when ROM bank selected.
 The value of Register 2 will be used as RAM bank number when reading/writing from/to RAM.

  If Upper ROM banking enabled, the 2 bit Register 2 will be used as most significant two bits to
 construct a 7 bit integer along with 5 bit Register 1 which will be the effective bank number that is selected:
 const effective_bank_number = (register_2 << 5) | register_1

All four registers are write only and their default values are 0.

  * 'x' means these bits are ignored, so register 1 is a 5 bit register
 ** This means this register can represent 32 different values. Each ROM banks is 16KB:
16KB * 32 == 512KB
This implies if the game cartridge ROM size less than or equal to 512KB then
no need to use Register 2 for "upper banking"
*** These bits and Register 1 creates 7 bit integer which can represent 128 == 2^7 states.
This implies mbc1 cartridges can hold at most 16KB * 128 == 2048KB == 2MB of bytes.

*/

namespace LR35902 {
namespace rv = ranges::views;
namespace mp = mpark::patterns;

mbc1::mbc1(std::vector<byte> other, const std::size_t RAM_size, const bool has_battery) :
    m_rom{std::move(other)},
    m_sram(RAM_size, byte{}),
    has_sram{static_cast<bool>(m_sram.size())},
    has_battery{has_battery} {}

// clang-format off
byte mbc1::readROM(const std::size_t index) const noexcept {
  using namespace mp;

  static const auto banked_rom_view = m_rom | rv::const_ | rv::chunk(rom_bank_size);

  return match(index)(
      pattern(arg).when(arg >= mmap::rom0 && arg < mmap::romx) = [&](auto index) { return m_rom[index]; },
      pattern(arg).when(arg >= mmap::romx && arg < mmap::romx_end) = [&](auto index) {
        const auto index_normalized = index % rom_bank_size;
        if(register_3 == 1) return banked_rom_view[register_1][index_normalized];
        return banked_rom_view[((register_2 << 5) | register_1)][index_normalized];
      });
}

void mbc1::writeROM(const std::size_t index, const byte b) noexcept {
  using namespace mp;
  // clang-format off
  match(index)(
      pattern(arg).when(arg >= 0x0000 && arg < 0x2000) = [&](auto) { register_0 = (b & 0x0f) == 0x0A; }, 
      pattern(arg).when(arg >= 0x2000 && arg < 0x4000) = [&](auto) { register_1 = b & 0x1f; if(register_1 == 0) ++register_1; },
      pattern(arg).when(arg >= 0x4000 && arg < 0x6000) = [&](auto) { register_2 = b & 0x3; },
      pattern(arg).when(arg >= 0x6000 && arg < 0x8000) = [&](auto) { register_3 = b & 0x01; }
  );
}

byte mbc1::readSRAM(std::size_t index) const noexcept {
  index = normalize_index(index, mmap::sram);
  if(register_0) {
    static const auto banked_ram_view = m_sram | rv::const_ | rv::chunk(sram_bank_size);

    if(register_3 == 1) return banked_ram_view[register_2][index];
    else return banked_ram_view[0][index];
  }

  else {
    return random_byte();
  }
}

void mbc1::writeSRAM(std::size_t index, const byte b) noexcept {
  index = normalize_index(index, mmap::sram);
  if(register_0) {
    static const auto banked_ram_view = m_sram | rv::chunk(sram_bank_size);

    if(register_3 == 1) banked_ram_view[register_2][index] = b;
    else banked_ram_view[0][index] = b;
  }
}

}
