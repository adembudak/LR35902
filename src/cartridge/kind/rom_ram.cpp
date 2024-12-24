#include <LR35902/cartridge/kind/rom_ram.h>
#include <LR35902/cartridge/kind/mbc_config.h>
#include <LR35902/config.h>
#include <LR35902/memory_map.h>

#include <cstddef>
#include <utility>

namespace LR35902 {

rom_ram::rom_ram(std::vector<byte> rom, const MBC_config& config) :
    m_rom{std::move(rom)},
    has_battery{config.has_battery} {}

byte rom_ram::readROM(const address_t index) const noexcept {
  return m_rom[index];
}

void rom_ram::writeROM(const address_t index, const byte b) noexcept {
  m_rom[index] = b;
}

byte rom_ram::readSRAM(const address_t index) const noexcept {
  return m_sram[index];
}

void rom_ram::writeSRAM(const address_t index, const byte b) noexcept {
  m_sram[index] = b;
}

}
