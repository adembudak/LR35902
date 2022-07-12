#include <LR35902/stubs/cartridge/kind/mbc1.h>

#include <cassert>

namespace LR35902 {

constexpr std::size_t rom_bank_size = 16_KiB;

mbc1::mbc1(const std::vector<byte> rom) :
    m_rom(std::move(rom)) {

  const auto rom_size = m_rom.size();
  if(rom_size <= 512_KiB) //
    secondary_bank = std::nullopt;

  // clang-format off
  if(rom_size == 32_KiB)       bank_normalize_mask = 0b0000'0001;
  else if(rom_size == 64_KiB)  bank_normalize_mask = 0b0000'0011;
  else if(rom_size == 128_KiB) bank_normalize_mask = 0b0000'0111;
  else if(rom_size == 256_KiB) bank_normalize_mask = 0b0000'1111;
  else if(rom_size == 512_KiB) bank_normalize_mask = 0b0001'1111;
  else if(rom_size == 1_MiB)   bank_normalize_mask = 0b0001'1111;
  else if(rom_size == 2_MiB)   bank_normalize_mask = 0b0001'1111;
  else assert(false);
  // clang-format on
}

byte mbc1::read(std::size_t index) const noexcept {
  index &= 0b0011'1111'1111'1111; // 14 bit index is used

  if(index < 0x4000) { // [0x0000,0x3fff], RO
    if(secondary_bank.has_value() && secondary_bank_enabled)
      return m_rom[((*secondary_bank << 5) * rom_bank_size) + index];
    return m_rom[index];
  }

  else if(index < 0x8000) { // [0x4000,0x7fff], RO
    if(primary_bank == 0) primary_bank = 1;

    if(secondary_bank.has_value())
      return m_rom[(((*secondary_bank << 5) | primary_bank) * rom_bank_size) + index];
    return m_rom[(primary_bank * rom_bank_size) + index];
  }

  else if(index >= 0xa000 && index < 0xc000) // [0xa000, 0xbfff], R/W. No ram in this cart
    return random_byte();

  else assert(false);
}

void mbc1::write(const std::size_t index, const byte b) noexcept {
  if(index < 0x2000) // [0x0000,0x1fff] WO. Ram enable, no ram in this cart, do nothing
    return;

  else if(index < 0x4000) { // [0x0000, 0x3fff], WO. primary bank select
    primary_bank = b & bank_normalize_mask;
    if(primary_bank == 0) // if bank number is 0, use 1 instead
      primary_bank = 1;
  }

  else if(index < 0x6000) { // [0x4000,0x5fff], WO. ram bank or secondary rom bank selection for rom size >
                            // 512_KiB. No ram in this type of cart so only used for secondary rom bank

    if(secondary_bank.has_value()) secondary_bank = b & 0b0000'0011; // 2 bit register, ignore other bits
  }

  else if(index < 0x8000) { // [0x6000,0x7fff] W0. Ram or secondary rom banking enable, no ram in this cart
                            // use always for rom banking mode
    secondary_bank_enabled = b & 0b0000'0001;
  }

  else if(index >= 0xa000 && index < 0xc000) { // [0xa000,0xbfff] W/R no ram in this cart, do nothing
    (void)index;                               // write ignored
    (void)b;
  }

  else
    assert(false);
}

byte &mbc1::operator[](std::size_t index) noexcept {
  index &= 0b0011'1111'1111'1111; // 14 bit index is used

  if(index < 0x4000) { // [0x0000,0x3fff], RO
    if(secondary_bank.has_value() && secondary_bank_enabled)
      return m_rom[((*secondary_bank << 5) * rom_bank_size) + index];
    return m_rom[index];
  }

  else if(index < 0x8000) { // [0x4000,0x7fff], RO
    if(primary_bank == 0) primary_bank = 1;

    if(secondary_bank.has_value())
      return m_rom[(((*secondary_bank << 5) | primary_bank) * rom_bank_size) + index];
    return m_rom[(primary_bank * rom_bank_size) + index];
  }

  else if(index >= 0xa000 && index < 0xc000) // [0xa000, 0xbfff], R/W. No ram in this cart
    return m_rom[0];

  else assert(false);
}

const byte *mbc1::data() const noexcept {
  return m_rom.data();
}

}
