#include <LR35902/config.h>
#include <LR35902/stubs/builtin/builtin.h>

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

byte BuiltIn::readHRAM(const std::size_t index) const noexcept {
  return m_hram[index];
}

void BuiltIn::writeHRAM(const std::size_t index, const byte b) noexcept {
  m_hram[index] = b;
}

}
