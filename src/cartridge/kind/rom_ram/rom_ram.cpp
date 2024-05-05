#include <LR35902/cartridge/kind/rom_ram/rom_ram.h>
#include <LR35902/config.h>
#include <LR35902/memory_map.h>

#include <cstddef>

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

}
