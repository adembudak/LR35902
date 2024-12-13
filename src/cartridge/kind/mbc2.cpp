#include <LR35902/cartridge/kind/mbc2.h>
#include <LR35902/config.h>
#include <LR35902/memory_map.h>

#include <range/v3/view/chunk.hpp>
#include <range/v3/view/const.hpp>

#include <cassert>
#include <cstddef>

// Verbatim implementation of
// https://gbdev.io/pandocs/MBC2.html

namespace LR35902 {
namespace rv = ranges::views;

mbc2::mbc2(std::vector<byte> rom, bool has_battery) :
    m_rom(std::move(rom)),
    has_battery{has_battery} {}

byte mbc2::readROM(const address_t index) const noexcept {
  if(index < mmap::rom0_end) {
    return m_rom[index];
  }

  else if(index < mmap::romx_end) {
    const std::size_t index_normalized = index % rom_bank_size;
    static const auto banked_rom_view = m_rom | rv::const_ | rv::chunk(rom_bank_size);
    return banked_rom_view[rom_bank][index_normalized];
  }

  else {
    assert(false);
  }
}

void mbc2::writeROM(const address_t index, const byte b) noexcept {
  if(index < mmap::rom0_end) {
    if(index & 0b1'0000'0000) {
      rom_bank = b & 0x0f;
      if(rom_bank == 0) ++rom_bank;
    } else {
      ram_enabled = b == 0x0A;
    }
  }

  else {
    assert(false);
  }
}

byte mbc2::readSRAM(address_t index) const noexcept {
  if(ram_enabled) {
    index = index % 512_B;
    return m_sram[index];
  }
  return random_byte();
}

void mbc2::writeSRAM(address_t index, const byte b) noexcept {
  if(ram_enabled) {
    index = index % 512_B;
    m_sram[index] = b & 0x0f;
  }
}

}
