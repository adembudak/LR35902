#include <LR35902/cartridge/kind/mbc2/mbc2.h>
#include <LR35902/config.h>
#include <LR35902/memory_map.h>

#include <cassert>
#include <cstddef>

namespace LR35902 {

mbc2::mbc2(std::vector<byte> rom) :
    m_rom(std::move(rom)) {}

byte mbc2::readROM(const std::size_t index) const noexcept {
  if(index < mmap::rom0_end) {
    return m_rom[index];
  }

  else if(index < mmap::romx_end) {
    const std::size_t normalized_index = index % rom_bank_size;
    return m_rom[upper_rom_bank * rom_bank_size + normalized_index];
  }

  else {
    assert(false);
  }
}

void mbc2::writeROM(const std::size_t index, const byte b) noexcept {
  if(index < mmap::rom0_end) {
    if(index & 0b1'0000'0000) //
      upper_rom_bank = b;
    else //
      ram_enabled = (b & 0x0f) == 0x0a;
  }

  else if(index >= mmap::sram && index < mmap::sram_end) {
    if(ram_enabled) {
      const std::size_t normalized_index = (index % rom_bank_size) % 0x200;
      const byte normalized_value = b & 0b0000'1111;
      m_sram[normalized_index] = normalized_value;
    }

    else {
      (void)index;
      (void)b;
    }
  }

  else {
    (void)index;
    (void)b;
  }
}

byte mbc2::readSRAM(const std::size_t index) const noexcept {
  static_assert((mmap::sram_end - mmap::sram) == sram_bank_size);
  static_assert((0xa200 - mmap::sram) == 512_B);
  static_assert(512_B * 16 == sram_bank_size);

  if(index < 0xa200) { // first 512_B is used
  }

  else if(index < mmap::sram_end) { // rest 15 * 512_B is used as "echo"
  }
}

void mbc2::writeSRAM(const std::size_t index, const byte b) noexcept {
  writeROM(index, b);
}

}
