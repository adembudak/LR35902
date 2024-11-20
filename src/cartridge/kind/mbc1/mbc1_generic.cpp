#include <LR35902/cartridge/kind/mbc1/mbc1_generic.h>
#include <LR35902/config.h>
#include <LR35902/memory_map.h>

#include <range/v3/view/chunk.hpp>
#include <range/v3/view/const.hpp>

#include <cstddef>
#include <vector>

// Verbatim implementation of:
// https://gbdev.io/pandocs/MBC1.html

/*
MBC1 Registers
---
 The term "register" might be misleading, these are not byte adressed registers,
instead attempt to write certain ranges of the read only memory will be interpreted
as numerical value and will be used as an indicator or a numerical value to be used.
e.g. a bank number.

Register 0:
   7   6   5   4   3   2   1   0
 ┌───┬───┬───┬───┬───┬───┬───┬───┐
 │*x │ x │ x │ x │   │   │   │   │ [0x0000, 0x2000)
 └───┴───┴───┴───┴───┴───┴───┴───┘
 RAM enable register. RAM enabled only when lower nibble of the written byte is 0x0A.
 Disabled otherwise. Effectively a boolean.

Register 1:
 ┌───┬───┬───┬───┬───┬───┬───┬───┐
 │ x │ x │ x │   │   │   │   │   │ [0x2000, 0x4000)
 └───┴───┴───┴───┴───┴───┴───┴───┘
 **Possible values: [0,32)
 Determines which ROM bank to select:
 if(register_3) // see Register 3
   mem[((register_2 << 5) | register_1) * 16_KiB) + index]
 else
   mem[(register_1 * 16_KiB) + index]

Register 2:
 ┌───┬───┬───┬───┬───┬───┬───┬───┐
 │ x │ x │ x │ x │ x │ x │   │   │ [0x4000, 0x6000]
 └───┴───┴───┴───┴───┴───┴───┴───┘
 ***Possible values: {0, 1, 2, 3}
 RAM bank or (depends on Register 3) Upper ROM bank number

Register 3:
 ┌───┬───┬───┬───┬───┬───┬───┬───┐
 │ x │ x │ x │ x │ x │ x │ x │   │ [0x6000, 0x8000]
 └───┴───┴───┴───┴───┴───┴───┴───┘
 Possible values:
  0 : Has two implication;
  - Upper ROM banking disabled, only Register 1 will be used when ROM bank selected.
  - RAM banking enabled, this 2 bits will be used as RAM bank number when
reading/writing from/to RAM.
  1 : Upper ROM banking enabled, the 2 bit Register 2 will be used as
most significant two bits to construct a 7 bit integer along with 5 bit Register 1 which will be the effective bank
number that is selected:
const effective_bank_number = (register_2 << 5) | register_1

All four registers are write only and their default values are 0.

  * 'x' means these bits are ignored, so register 1 is a 5 bit register)
 ** This means this register can represent 32 different values. Each ROM banks are 16KB:
16KB * 32 == 512KB
This implies if the game cartridge ROM size less than or equal to 512KB then
no need to use Register 2 for "upper banking"
*** These bits and Register 1 creates 7 bit integer which can represent 128 == 2^7 states.
This implies mbc1 cartridges can hold at most 16KB * 128 == 2048KB == 2MB of bytes.

*/

namespace LR35902 {
namespace rg = ranges;
namespace rv = rg::views;

constexpr std::size_t upper_bank_increment = 32;
static_assert(32 * 16_KiB == 512_KiB);

mbc1_generic::mbc1_generic(std::vector<byte> other, const std::size_t RAM_size, const bool has_battery) :
    m_rom{std::move(other)},
    m_sram(RAM_size, byte{}),
    has_sram{static_cast<bool>(m_sram.size())},
    has_battery{has_battery} {}

byte mbc1_generic::readROM(const std::size_t index) const noexcept {
  /*
const rg::chunk_view<rg::chunk_view<
rg::const_view<rg::ref_view<const std::vector<unsigned char, std::allocator<unsigned char>>>>>>
portions = m_rom                      //
           | rv::const_               //
           | rv::chunk(rom_bank_size) //
           | rv::chunk(upper_bank_increment);
           */

  if(index < mmap::rom0_end) {
    return m_rom[index];
  }

  else if(index < mmap::romx_end) {
    static const auto portions = m_rom                      //
                                 | rv::const_               //
                                 | rv::chunk(rom_bank_size) //
                                 | rv::chunk(upper_bank_increment);

    if(register_3 == 0) return portions[0][register_1][index];
    else return portions[register_2][register_1][index];
  }

  else {
    return random_byte();
  }
}

void mbc1_generic::writeROM(const std::size_t index, const byte b) noexcept {
  if(index < 0x2000) { // ram gate
    register_0 = (b & 0x0f) == 0x0A;
  }

  else if(index < 0x4000) {
    byte bank = b;
    if(bank == 0x00) ++bank;
    register_1 = bank & 0x1f;
  }

  else if(index < 0x6000) {
    register_2 = b & 0x3;
  }

  else if(index < 0x8000) { // possible values [0, 1]. When 0, register_2 is used for rom banking, when 1,
                            // register_2 is used for ram banking
    register_3 = b & 0x01;
  }

  else {
    (void)index;
    (void)b;
  }
}

byte mbc1_generic::readSRAM(const std::size_t index) const noexcept {
  if(register_0) {
    static const auto portion = m_sram | rv::chunk(ram_bank_size);

    if(register_3 == 1) return portion[register_2][index];
    else return portion[0][index];
  }

  else {
    return random_byte();
  }
}

void mbc1_generic::writeSRAM(const std::size_t index, const byte b) noexcept {
  if(register_0) {
    static const auto portion = m_sram | rv::chunk(ram_bank_size);
    if(register_3 == 1) portion[register_2][index] = b;
    else portion[0][index] = b;
  }
}

}
