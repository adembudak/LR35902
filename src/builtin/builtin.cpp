#include <LR35902/builtin/builtin.h>
#include <LR35902/config.h>
#include <LR35902/memory_map.h>

#include <cstdint>

namespace LR35902 {

byte BuiltIn::readWRAM(address_t index) const noexcept {
  index = normalize_index(index, mmap::wram0);
  return m_wram[index];
}

void BuiltIn::writeWRAM(address_t index, const byte b) noexcept {
  index = normalize_index(index, mmap::wram0);
  m_wram[index] = b;
}

byte BuiltIn::readEcho(address_t index) const noexcept {
  index = normalize_index(index, mmap::echo);
  return m_echo[index];
}

void BuiltIn::writeEcho(address_t index, const byte b) noexcept {
  index = normalize_index(index, mmap::echo);
  m_echo[index] = b;
}

byte BuiltIn::readNoUsable(address_t index) const noexcept {
  index = normalize_index(index, mmap::noUse);
  return m_noUsable[index];
}

void BuiltIn::writeNoUsable(address_t index, const byte b) noexcept {
  index = normalize_index(index, mmap::noUse);
  m_noUsable[index] = b;
}

byte BuiltIn::readHRAM(address_t index) const noexcept {
  index = normalize_index(index, mmap::hram);
  return m_hram[index];
}

void BuiltIn::writeHRAM(address_t index, const byte b) noexcept {
  index = normalize_index(index, mmap::hram);
  m_hram[index] = b;
}

void BuiltIn::reset() noexcept {
  m_wram.fill(byte{});
  m_echo.fill(byte{});
  m_noUsable.fill(byte{});
  m_hram.fill(byte{});
}

}
