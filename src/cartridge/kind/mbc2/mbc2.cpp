#include <LR35902/cartridge/kind/mbc2/mbc2.h>
#include <LR35902/config.h>
#include <LR35902/memory_map.h>

#include <cassert>
#include <cstdint>

namespace LR35902 {

constexpr std::size_t rom_bank_size = 0x4000;
constexpr std::size_t ram_bank_size = 0x2000;

void mbc2::bank_t::setPrimaryBank(const byte b) noexcept {
  primary = b & primary_bank_mask;
  if(primary == 0) ++primary;
}

std::size_t mbc2::bank_t::value() const noexcept {
  return primary * rom_bank_size;
}

mbc2::mbc2(std::vector<byte> rom) :
    m_rom(std::move(rom)) {
  switch(std::size(m_rom)) {
  case 256_KiB: break;
  case 128_KiB: bank.primary_bank_mask >>= 1; break;
  case 64_KiB: bank.primary_bank_mask >>= 2; break;
  }
}

byte mbc2::readROM(const std::size_t index) const noexcept {
  if(index < mmap::rom0_end) {
    return m_rom[index];
  }

  else if(index < mmap::romx_end) {
    const std::size_t normalized_index = index % rom_bank_size;
    return m_rom[bank.value() + normalized_index];
  }

  else if(index >= mmap::sram && index < mmap::sram_end) {
    if(bank.ramg) {
      const std::size_t normalized_index = (index % rom_bank_size) % 0x200;
      return m_sram[normalized_index];
    }

    return random_byte();
  }

  else {
    assert(false);
  }
}

void mbc2::writeROM(const std::size_t index, const byte b) noexcept {
  if(index < mmap::rom0_end) {
    if(index & 0b1'0000'0000) //
      bank.setPrimaryBank(b);
    else //
      bank.ramg = (b & 0x0f) == 0x0a;

  }

  else if(index >= mmap::sram && index < mmap::sram_end) {
    if(bank.ramg) {
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
  return readROM(index);
}

void mbc2::writeSRAM(const std::size_t index, const byte b) noexcept {
  writeROM(index, b);
}

}
