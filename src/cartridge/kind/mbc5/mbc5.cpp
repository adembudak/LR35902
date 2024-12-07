#include <LR35902/cartridge/kind/mbc5/mbc5.h>
#include <LR35902/config.h>
#include <LR35902/memory_map.h>

#include <mpark/patterns/match.hpp>
#include <mpark/patterns/when.hpp>

#include <range/v3/view/chunk.hpp>
#include <range/v3/view/const.hpp>

#include <cassert>
#include <cstddef>

// Verbatim implementation of:
// https://archive.org/details/GameBoyProgManVer1.1/page/n223/mode/1up
// https://gbdev.io/pandocs/MBC5.html
// https://gekkio.fi/files/gb-docs/gbctr.pdf

namespace LR35902 {
namespace rv = ranges::views;
namespace mp = mpark::patterns;

mbc5::mbc5(std::vector<byte> other, std::size_t RAM_size, bool has_battery, bool has_rumble) :
    m_rom{std::move(other)},
    m_sram(RAM_size),
    has_battery{has_battery},
    has_rumble{has_rumble} {}

static_assert((0b1 << 8) == 0b1'0000'0000);

// clang-format off
byte mbc5::readROM(const std::size_t index) const noexcept {
  using namespace mp;
  static const auto banked_rom_view = m_rom | rv::const_ | rv::chunk(rom_bank_size);

  return match(index)(
      pattern(arg).when(arg >= mmap::rom0 && arg < mmap::romx) = [&](auto index) { return m_rom[index]; },
      pattern(arg).when(arg >= mmap::romx && arg < mmap::romx_end) = [&](auto index) {
            index = normalize_index(index, mmap::romx);
            return banked_rom_view[((romb_1 << 8) | romb_0)][index]; }
      );
}

void mbc5::writeROM(const std::size_t index, const byte b) noexcept {
  using namespace mp;

  match(index)(
      pattern(_).when(_ >= 0x0000 && _ < 0x2000) = [&] { ramg = has_rumble ? (b == 0x0A) : ((b & 0x0f) == 0x0A); },
      pattern(_).when(_ >= 0x2000 && _ < 0x3000) = [&] { romb_0 = b; },
      pattern(_).when(_ >= 0x3000 && _ < 0x4000) = [&] { romb_1 = b & 0x01; },
      pattern(_).when(_ >= 0x4000 && _ < 0x6000) = [&] { ramb = b & 0x0f; },
      pattern(_) = [] {}
      );
}
// clang-format on

[[nodiscard]] byte mbc5::readSRAM(std::size_t index) const noexcept {
  index = normalize_index(index, mmap::sram);
  static const auto banked_ram_view = m_sram | rv::const_ | rv::chunk(sram_bank_size);
  if(ramg) {
    return banked_ram_view[ramb][index];
  } else {
    return random_byte();
  }
}

void mbc5::writeSRAM(std::size_t index, const byte b) noexcept {
  index = normalize_index(index, mmap::sram);
  static const auto banked_ram_view = m_sram | rv::chunk(sram_bank_size);
  if(ramg) {
    banked_ram_view[ramb][index] = b;
  }
}

}
