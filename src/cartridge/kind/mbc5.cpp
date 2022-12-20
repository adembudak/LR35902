#include <LR35902/cartridge/kind/mbc5.h>
#include <LR35902/config.h>
#include <LR35902/memory_map.h>

#include <cassert>
#include <cstddef>

namespace LR35902 {

constexpr std::size_t rom_bank_size = 0x4000;

void mbc5::bank_t::setPrimaryBank(const byte b) noexcept {
  this->primary = b & primary_bank_mask;
}

void mbc5::bank_t::setSecondaryBank(const byte b) noexcept {
  secondary = b & secondary_bank_mask;
}

std::size_t mbc5::bank_t::value() const noexcept {
  const std::size_t effective_bank = (secondary << 8) | primary;
  return effective_bank * rom_bank_size;
}

mbc5::mbc5(std::vector<byte> other) :
    m_rom{std::move(other)} {

  switch(std::size(m_rom)) {
  case 8_MiB: //
    break;

  case 4_MiB: //
    bank.secondary_bank_mask >>= 1;
    break;

  case 2_MiB:
    bank.secondary_bank_mask >>= 1;
    bank.primary_bank_mask >>= 1;
    break;

  case 1_MiB:
    bank.secondary_bank_mask >>= 1;
    bank.primary_bank_mask >>= 2;
    break;

  case 512_KiB:
    bank.secondary_bank_mask >>= 1;
    bank.primary_bank_mask >>= 3;
    break;

  case 256_KiB:
    bank.secondary_bank_mask >>= 1;
    bank.primary_bank_mask >>= 4;
    break;

  case 128_KiB:
    bank.secondary_bank_mask >>= 1;
    bank.primary_bank_mask >>= 5;
    break;

  case 64_KiB:
    bank.secondary_bank_mask >>= 1;
    bank.primary_bank_mask >>= 6;
    break;

  case 32_KiB:
    bank.secondary_bank_mask >>= 1;
    bank.primary_bank_mask >>= 7;
    break;

  default: //
    assert(false);
  }
}

byte mbc5::readROM(const std::size_t index) const noexcept {
  if(index < mmap::rom0_end) {
    return m_rom[index];
  }

  else if(index < mmap::romx_end) {
    const std::size_t index_normalized = index % rom_bank_size;
    return m_rom[bank.value() + index_normalized];
  }

  else if(index >= mmap::sram && index < mmap::sram_end) {
    return random_byte();
  }

  else {
    assert(false);
  }
}

void mbc5::writeROM(const std::size_t index, const byte b) noexcept {
  if(index < 0x2000) {
    (void)index;
    (void)b;
  }

  else if(index < 0x3000) {
    bank.setPrimaryBank(b);
  }

  else if(index < 0x4000) {
    bank.setSecondaryBank(b);
  }

  else if(index < 0x6000) {
    (void)index;
    (void)b;
  }

  else if(index >= mmap::sram && index < mmap::sram_end) {
    (void)index;
    (void)b;
  }

  else {
    (void)index;
    (void)b;
  }
}

}
