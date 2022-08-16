#include <LR35902/builtin/builtin.h>
#include <LR35902/config.h>
#include <LR35902/memory_map.h>

#include <cassert>
#include <cstdint>

namespace LR35902 {

byte BuiltIn::readWRAM(const std::size_t index) const noexcept {
  return m_wram[index];
}

void BuiltIn::writeWRAM(const std::size_t index, const byte b) noexcept {
  m_wram[index] = b;
}

byte BuiltIn::readEcho(const std::size_t index) const noexcept {
  return m_echo[index];
}

void BuiltIn::writeEcho(const std::size_t index, const byte b) noexcept {
  m_echo[index] = b;
}

byte BuiltIn::readNoUsable(const std::size_t index) const noexcept {
  return m_noUsable[index];
}

void BuiltIn::writeNoUsable(const std::size_t index, const byte b) noexcept {
  m_noUsable[index] = b;
}

byte BuiltIn::readHRAM(const std::size_t index) const noexcept {
  return m_hram[index];
}

void BuiltIn::writeHRAM(const std::size_t index, const byte b) noexcept {
  m_hram[index] = b;
}

byte &BuiltIn::operator[](const std::size_t index) noexcept {
  if(index < wramx_end) return m_wram[index - wram0];
  else if(index < echo_end) return m_echo[index - echo];
  else if(index < noUsable_end) return m_noUsable[index - noUsable];
  else if(index < hram_end) return m_hram[index - hram];
  else assert(false);
}

void BuiltIn::reset() noexcept {
  m_wram.fill(byte{});
  m_echo.fill(byte{});
  m_noUsable.fill(byte{});
  m_hram.fill(byte{});
}

}
