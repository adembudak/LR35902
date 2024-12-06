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

namespace LR35902 {
namespace rv = ranges::views;
namespace mp = mpark::patterns;

mbc5::mbc5(std::vector<byte> other, std::size_t RAM_size, bool has_battery, bool has_rumble) :
    m_rom{std::move(other)},
    m_sram(RAM_size),
    has_battery{has_battery},
    has_rumble{has_rumble} {}

static_assert((0b1 << 8) == 0b1'0000'0000);

byte mbc5::readROM(const std::size_t index) const noexcept {
  using namespace mp;
  static const auto banked_rom_view = m_rom | rv::const_ | rv::chunk(rom_bank_size);

  return match(index)(
      pattern(arg).when(arg >= mmap::rom0 && arg < mmap::romx) = [&](auto index) { return m_rom[index]; },
      pattern(arg).when(arg >= mmap::romx && arg < mmap::romx_end) =
          [&](auto index) {
            index = normalize_index(index, mmap::romx);
            return banked_rom_view[((romb_1 << 8) | romb_0)][index];
          });
}

void mbc5::writeROM(const std::size_t index, const byte b) noexcept {
  using namespace mp;

  match(index)(
      pattern(arg).when(arg >= 0x0000 && arg < 0x2000) = [&](auto index) { ramg = (b & 0x0f) == 0x0A; },
      pattern(arg).when(arg >= 0x2000 && arg < 0x3000) = [&](auto index) { romb_0 = b; },
      pattern(arg).when(arg >= 0x3000 && arg < 0x4000) = [&](auto index) { romb_1 = b & 0x01; },
      pattern(arg).when(arg >= 0x4000 && arg < 0x6000) = [&](auto index) { ramb = b & 0x0f; });
}

[[nodiscard]] byte mbc5::readSRAM(std::size_t index) const noexcept {
  static const auto portion = m_sram | rv::chunk(sram_bank_size);

  if(ramg) {

  } else {
    return random_byte();
  }
}

void mbc5::writeSRAM(std::size_t index, const byte b) noexcept {
  if(ramg) {
  }
}

}
