#include <LR35902/config.h>
#include <LR35902/core/immediate/n16.h>
#include <LR35902/core/registers/r16.h>
#include <LR35902/core/registers/r8.h>

#include <cstdint>

namespace LR35902 {

r16 &r16::operator=(const n16 nn) {
  m_lo = nn.m_data & 0b0000'0000'1111'1111;
  m_hi = (nn.m_data & 0b1111'1111'0000'0000) >> 8;

  return *this;
}

std::uint16_t r16::data() const noexcept {
  return std::uint16_t(m_hi.data() << 8 | m_lo.data());
}

byte &r16::operator*() noexcept {
  return m_bus.read_write(data());
}

}
