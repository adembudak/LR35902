#include <LR35902/stubs/cartridge/cartridge.h>

#include <fstream>

namespace LR35902 {

void Cartridge::load(const char *romfile) {
  std::ifstream fin{romfile};
  fin.exceptions(std::ifstream::failbit);
  m_rom.assign(std::istreambuf_iterator<char>{fin}, {});
}

byte &Cartridge::operator[](const std::size_t index) noexcept {
  return m_rom[index];
}

byte Cartridge::readSRAM(const std::size_t index) const noexcept {
  return m_sram[index];
}

void Cartridge::writeSRAM(const std::size_t index, const byte b) noexcept {
  m_sram[index] = b;
}

byte Cartridge::readROM(const std::size_t index) const noexcept {
  return m_rom[index];
}

void Cartridge::writeROM(const std::size_t index, const byte b) noexcept {
  m_rom[index] = b;
}

auto Cartridge::data() const noexcept -> std::vector<byte> {
  return m_rom;
}

}
