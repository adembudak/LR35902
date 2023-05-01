#include <LR35902/cartridge/kind/mbc1_ram/mbc1_ram.h>
#include <LR35902/config.h>
#include <LR35902/memory_map.h>

#include <cassert>
#include <cstddef>

namespace LR35902 {

constexpr std::size_t rom_bank_size = 0x4000;
constexpr std::size_t ram_bank_size = 0x2000;

void mbc1_ram::bank_t::setPrimaryBank(const byte b) noexcept {
  primary = b & primary_bank_mask;

  if(primary == 0) ++primary;
}

void mbc1_ram::bank_t::setSecondaryBank(const byte b) noexcept {
  secondary = b & secondary_bank_mask;
}

std::size_t mbc1_ram::bank_t::value() const noexcept {
  const std::size_t effective_bank = (secondary << 5) | primary;
  return effective_bank * rom_bank_size;
}

mbc1_ram::mbc1_ram(std::vector<byte> other, const std::size_t ram_size) :
    m_rom(std::move(other)),
    m_sram(ram_size) {

  switch(std::size(m_rom)) {
  case 2_MiB: break;
  case 1_MiB: break;
  case 512_KiB: break;
  case 256_KiB: bank.primary_bank_mask >>= 1; break;
  case 128_KiB: bank.primary_bank_mask >>= 2; break;
  case 64_KiB: bank.primary_bank_mask >>= 3; break;
  case 32_KiB: bank.primary_bank_mask >>= 4; break;
  default: assert(false);
  }
}

byte mbc1_ram::readROM(const std::size_t index) const noexcept {
  if(index < mmap::rom0_end) {
    if(mode == mode_t::ram_banking) //
      return m_rom[((bank.secondary << 5) * rom_bank_size) + index];

    return m_rom[index];
  }

  else if(index < mmap::romx_end) {
    const std::size_t index_normalized = index % rom_bank_size;

    if(mode == mode_t::rom_banking) //
      return m_rom[bank.value() + index_normalized];
    else //
      return m_rom[(bank.primary * rom_bank_size) + index_normalized];
  }

  else if(index >= mmap::sram && index < mmap::sram_end) {
    if(bank.ramg) {
      const std::size_t index_normalized = index % ram_bank_size;
      if(mode == mode_t::ram_banking) //
        return m_sram[(bank.secondary * ram_bank_size) + index_normalized];
      return m_sram[index_normalized];
    }

    return random_byte();
  }

  else {
    assert(false);
  }
}

void mbc1_ram::writeROM(const std::size_t index, const byte b) noexcept {
  if(index < 0x2000) {
    bank.ramg = (b & 0x0f) == 0x0a; // 0x0A is magic constant enables ram.
  }

  else if(index < 0x4000) {
    bank.setPrimaryBank(b);
  }

  else if(index < 0x6000) {
    bank.setSecondaryBank(b);
  }

  else if(index < 0x8000) {
    mode = (b & 0b0000'0001) ? mode_t::ram_banking : mode_t::rom_banking;
  }

  else if(index >= mmap::sram && index < mmap::sram_end) {
    if(bank.ramg) {
      const std::size_t index_normalized = index % ram_bank_size;
      if(mode == mode_t::ram_banking) //
        m_sram[(bank.secondary * ram_bank_size) + index_normalized] = b;
      else //
        m_sram[index_normalized] = b;
    } else {
      (void)index;
      (void)b;
    }
  }

  else {
    assert(false);
  }
}

byte mbc1_ram::readSRAM(const std::size_t index) const noexcept {
  return readROM(index);
}

void mbc1_ram::writeSRAM(const std::size_t index, const byte b) noexcept {
  writeROM(index, b);
}

}
