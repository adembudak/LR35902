#include <LR35902/cartridge/kind/mbc1_2mb.h>
#include <LR35902/config.h>
#include <LR35902/memory_map.h>

#include <range/v3/view/chunk.hpp>

#include <cstddef>
#include <vector>

// Verbatim implementation of:
//

namespace rv = ranges::views;

namespace LR35902 {

constexpr std::size_t rom_bank_size = 16_KiB;
constexpr std::size_t upper_bank_increment = 32;
static_assert(32 * 16_KiB == 512_KiB);

mbc1_2mb::mbc1_2mb(std::vector<byte> other) :
    m_rom{std::move(other)} {}

byte mbc1_2mb::readROM(const std::size_t index) const noexcept {
  if(index < mmap::rom0_end) {
    return m_rom[index];
  }

  else if(index < mmap::romx_end) {
    const auto portions = m_rom | rv::chunk(rom_bank_size) | rv::chunk(upper_bank_increment);

    if(register_3 == 0) return portions[register_2][register_1][index];
    else return portions[0][register_1][index];

  }

  else {
    return random_byte();
  }
}

void mbc1_2mb::writeROM(const std::size_t index, const byte b) noexcept {
  if(index < 0x2000) { // ram gate
    register_0 = b == 0x0A;
  }

  else if(index < 0x4000) { // rom bank code, possible values [1,31]
    int bank = b;
    if(bank == 0x00) ++bank;
    register_1 = bank & 0x1f;
  }

  else if(index < 0x6000) { // behavior depends on register_3, possible values [0, 1, 2, 3]
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

}
