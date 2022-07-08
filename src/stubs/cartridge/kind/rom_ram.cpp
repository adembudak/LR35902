#include <LR35902/config.h>
#include <LR35902/stubs/cartridge/kind/rom_ram.h>

namespace LR35902 {

byte rom_ram::readROM(const std::size_t index) const noexcept {
  return m_rom[index];
}

void rom_ram::writeROM(const std::size_t index, const byte b) noexcept {
  m_rom[index] = b;
}

byte rom_ram::readSRAM(const std::size_t index) const noexcept {
  return m_sram[index];
}

void rom_ram::writeSRAM(const std::size_t index, const byte b) noexcept {
  m_sram[index] = b;
}

auto rom_ram::data() const noexcept -> decltype(m_rom) {
  return m_rom;
}

}
