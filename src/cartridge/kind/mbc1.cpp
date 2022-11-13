#include <LR35902/cartridge/kind/mbc1.h>
#include <LR35902/config.h>

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

mbc1::mbc1(std::vector<byte> another) :
    m_rom{std::move(another)} {

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
  if(index < 0x4000) {
    return m_rom[index];
  }

  else if(index < 0x8000) {
    const std::size_t index_normalized = index % rom_bank_size;
    return m_rom[bank.value() + index_normalized];
  }

  else if(index >= 0xa000 && index < 0xc000) {
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

  else if(index >= 0xa000 && index < 0xc000) {
    (void)index;
    (void)b;
  }

  else {
    assert(false);
  }
}

const byte *mbc1::data() const noexcept {
  return m_rom.data();
}

const std::size_t mbc1::size() const noexcept {
  return m_rom.size();
}
}
