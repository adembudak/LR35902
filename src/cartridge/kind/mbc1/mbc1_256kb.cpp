#include <LR35902/cartridge/kind/mbc1/mbc1_256kb.h>
#include <LR35902/config.h>
#include <LR35902/memory_map.h>

#include <range/v3/view/chunk.hpp>

#include <cstddef>
#include <vector>

namespace rv = ranges::views;

namespace LR35902 {

constexpr std::size_t rom_bank_size = 16_KiB;

mbc1_256kb::mbc1_256kb(std::vector<byte> other) :
    m_rom{std::move(other)} {}

byte mbc1_256kb::readROM(const std::size_t index) const noexcept {
  if(index < mmap::rom0_end) {
    return m_rom[index];
  }

  else if(index < mmap::romx_end) {
    const auto portions = m_rom | rv::chunk(rom_bank_size);

    const std::size_t normalized_index = index % rom_bank_size;
    return portions[register_1][normalized_index];
  }

  else {
    return random_byte();
  }
}

void mbc1_256kb::writeROM(const std::size_t index, const byte b) noexcept {
  if(index < 0x2000) { // ram gate, not usable, no ram
    register_0 = b == 0x0A;
  }

  else if(index < 0x4000) { // rom bank code, possible values [1,15]
    int bank = b;
    if(bank == 0x00) ++bank;
    register_1 = bank & 0x0f;
  }

  else if(index < 0x6000) { // behavior depends on register_3, not usable
    register_2 = b & 0x3;
  }

  else if(index < 0x8000) { // possible values [0, 1]. When 0, register_2 is used for rom banking, when 1,
                            // register_2 is used for ram banking, not usable
    register_3 = b & 0x01;
  }

  else {
    (void)index;
    (void)b;
  }
}

}
