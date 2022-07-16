#include <LR35902/config.h>
#include <LR35902/memory_map.h>
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

byte &rom_ram::operator[](const std::size_t index) noexcept {
  if(index < romx_end) //
    return m_rom[index];
  return m_sram[index - sram];
}

const byte *rom_ram::data() const noexcept {
  return m_rom.data();
}

std::size_t rom_ram::size() const noexcept {
  return m_rom.size();
}

[[nodiscard]] std::size_t rom_ram::RAMSize() const noexcept {
  return m_sram.size();
}

[[nodiscard]] const byte *rom_ram::RAMData() const noexcept {
  return m_sram.data();
}

}
