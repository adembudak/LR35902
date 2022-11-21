#include <LR35902/cartridge/kind/mbc1.h>
#include <LR35902/config.h>
#include <LR35902/memory_map.h>

#include <cassert>
#include <cstddef>

namespace LR35902 {

constexpr std::size_t rom_bank_size = 0x4000;

void mbc1::bank_t::setPrimaryBank(const byte b) noexcept {
  primary = b & primary_bank_mask;

  if(primary == 0) ++primary;
}

void mbc1::bank_t::setSecondaryBank(const byte b) noexcept {
  secondary = b & secondary_bank_mask;
}

std::size_t mbc1::bank_t::value() const noexcept {
  const std::size_t effective_bank = (secondary << 5) | primary;
  return effective_bank * rom_bank_size;
}

mbc1::mbc1(std::vector<byte> other) :
    m_rom{std::move(other)} {

  switch(std::size(m_rom)) {
  case 2_MiB: //
    break;

  case 1_MiB: //
    bank.secondary_bank_mask >>= 1;
    break;

  case 512_KiB: //
    bank.secondary_bank_mask >>= 2;
    break;

  case 256_KiB:
    bank.secondary_bank_mask >>= 2;
    bank.primary_bank_mask >>= 1;
    break;

  case 128_KiB:
    bank.secondary_bank_mask >>= 2;
    bank.primary_bank_mask >>= 2;
    break;

  case 64_KiB:
    bank.secondary_bank_mask >>= 2;
    bank.primary_bank_mask >>= 3;
    break;

  case 32_KiB:
    bank.secondary_bank_mask >>= 2;
    bank.primary_bank_mask >>= 4;
    break;

  default: //
    assert(false);
  }
}

byte mbc1::readROM(const std::size_t index) const noexcept {
  if(index < rom0_end) {
    return m_rom[index];
  }

  else if(index < romx_end) {
    const std::size_t index_normalized = index % rom_bank_size;
    return m_rom[bank.value() + index_normalized];
  }

  else if(index >= sram && index < sram_end) {
    return random_byte();
  }

  else {
    assert(false);
  }
}

void mbc1::writeROM(const std::size_t index, const byte b) noexcept {
  if(index < 0x2000) {
    (void)index;
    (void)b;
  }

  else if(index < 0x4000) {
    bank.setPrimaryBank(b);
  }

  else if(index < 0x6000) {
    bank.setSecondaryBank(b);
  }

  else if(index < 0x8000) {
    (void)index;
    (void)b;
  }

  else if(index >= sram && index < sram_end) {
    (void)index;
    (void)b;
  }

  else {
    assert(false);
  }
}

}
