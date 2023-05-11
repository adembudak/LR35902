#include <LR35902/cartridge/kind/mbc1_ram/mbc1_32kb_32kb.h>
#include <LR35902/config.h>
#include <LR35902/memory_map.h>

#include <range/v3/view/chunk.hpp>

#include <cstddef>
#include <vector>

namespace rv = ranges::views;

namespace LR35902 {

constexpr std::size_t rom_bank_size = 16_KiB;
constexpr std::size_t ram_bank_size = 8_KiB;

mbc1_32kb_32kb::mbc1_32kb_32kb(std::vector<byte> other) :
    m_rom{std::move(other)} {}

byte mbc1_32kb_32kb::readROM(const std::size_t index) const noexcept {
  if(index < mmap::rom0_end) {
    return m_rom[index];
  }

  else if(index < mmap::romx_end) {
    const auto portions = m_rom | rv::chunk(rom_bank_size);

    const std::size_t normalized_index = index % rom_bank_size;
    return portions[1][normalized_index];
  }

  else {
    return random_byte();
  }
}

void mbc1_32kb_32kb::writeROM(const std::size_t index, const byte b) noexcept {
  if(index < 0x2000) { // ram gate
    register_0 = b == 0x0A;
  }

  else if(index < 0x4000) { // rom bank code, possible values [1]
    int bank = b;
    if(bank == 0x00) ++bank;
    register_1 = bank & 0x01;
  }

  else if(index < 0x6000) { // behavior depends on register_3, [0,3]
    register_2 = b & 0x3;
  }

  else if(index < 0x8000) { // possible values [0, 1].
                            // When 0, register_2 is used for ROM banking,
                            // when 1, register_2 is used for RAM banking
    register_3 = b & 0x01;
  }

  else {
    (void)index;
    (void)b;
  }
}

byte mbc1_32kb_32kb::readSRAM(const std::size_t index) const noexcept {
  if(register_0) {
    const auto portion = m_sram | rv::chunk(ram_bank_size);
    const auto normalized_index = index % ram_bank_size;

    if(register_3 == 1) return portion[register_2][normalized_index];
    else return portion[0][normalized_index];
  }

  else {
    return random_byte();
  }
}

void mbc1_32kb_32kb::writeSRAM(const std::size_t index, const byte b) noexcept {
  if(register_0) {
    const auto portion = m_sram | rv::chunk(ram_bank_size);
    const auto normalized_index = index % ram_bank_size;

    if(register_3 == 1) portion[register_2][normalized_index] = b;
    else portion[0][normalized_index] = b;
  }
}

}
