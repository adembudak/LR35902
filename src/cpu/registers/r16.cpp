#include <LR35902/config.h>
#include <LR35902/cpu/immediate/n16.h>
#include <LR35902/cpu/registers/r16.h>
#include <LR35902/cpu/registers/r8.h>

#include <cstdint>

namespace LR35902 {

r16 &r16::operator=(const n16 nn) noexcept {
  m_lo = nn.m_data & 0b0000'0000'1111'1111;
  m_hi = (nn.m_data & 0b1111'1111'0000'0000) >> 8;

  return *this;
}

r8 r16::lo() const noexcept {
  return m_lo;
}

r8 r16::hi() const noexcept {
  return m_hi;
}

std::uint16_t r16::data() const noexcept {
  return std::uint16_t(m_hi.data() << 8 | m_lo.data());
}

byte &r16::operator*() noexcept {
  return m_bus.read_write(data());
}

r16 &r16::operator++() noexcept {
  if(m_hi != r8::max() && m_lo == r8::max()) {
    m_lo = r8::min();
    m_hi++;
  } else if(m_hi == r8::max() && m_lo == r8::max()) {
    m_hi = r8::min();
    m_lo = r8::min();
  } else {
    m_lo++;
  }

  return *this;
}

r16 &r16::operator--() noexcept {
  if(m_lo != r8::min()) {
    m_lo--;
  } else if(m_lo == r8::min() && m_hi != r8::min()) {
    m_hi--;
    m_lo = r8::max();
  } else if(m_lo == r8::min() && m_hi == r8::min()) {
    m_hi = r8::max();
    m_lo = r8::max();
  }

  return *this;
}

r16 &r16::operator+=(const r16 rr) noexcept {
  const std::uint16_t temp = data() + rr.data();
  m_lo = temp & 0b0000'0000'1111'1111;
  m_hi = (temp & 0b1111'1111'0000'0000) >> 8;

  return *this;
}

r16 &r16::operator-=(const r16 rr) noexcept {
  const std::uint16_t temp = data() - rr.data();
  m_lo = temp & 0b0000'0000'1111'1111;
  m_hi = (temp & 0b1111'1111'0000'0000) >> 8;

  return *this;
}

r16 &r16::operator+=(const n16 nn) noexcept {
  const std::uint16_t temp = data() + nn.m_data;
  m_lo = temp & 0b0000'0000'1111'1111;
  m_hi = (temp & 0b1111'1111'0000'0000) >> 8;

  return *this;
}

r16 &r16::operator-=(const n16 nn) noexcept {
  const std::uint16_t temp = data() - nn.m_data;
  m_lo = temp & 0b0000'0000'1111'1111;
  m_hi = (temp & 0b1111'1111'0000'0000) >> 8;

  return *this;
}

}
