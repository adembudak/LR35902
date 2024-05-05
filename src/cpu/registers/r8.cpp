#include <LR35902/config.h>
#include <LR35902/cpu/immediate/n8.h>
#include <LR35902/cpu/registers/r8.h>

#include <cstdint>

namespace LR35902 {
r8 &r8::operator=(const byte b) noexcept {
  m_data = b;
  return *this;
}

r8 &r8::operator=(const n8 n) noexcept {
  m_data = n.m_data;
  return *this;
}

std::uint8_t r8::data() const noexcept {
  return m_data;
}

std::uint8_t r8::lowNibble() const noexcept {
  return m_data & 0b0000'1111;
}

std::uint8_t r8::highNibble() const noexcept {
  return (m_data & 0b1111'0000) >> 4;
}

r8 &r8::operator++() noexcept {
  ++m_data;
  return *this;
}

r8 r8::operator++(int) noexcept {
  const auto temp = *this;
  ++m_data;
  return temp;
}

r8 &r8::operator--() noexcept {
  --m_data;
  return *this;
}

r8 r8::operator--(int) noexcept {
  const auto temp = *this;
  --m_data;
  return temp;
}

r8 &r8::operator+=(const r8 r) noexcept {
  m_data += r.data();
  return *this;
}

r8 &r8::operator-=(const r8 r) noexcept {
  m_data -= r.data();
  return *this;
}

r8 &r8::operator+=(const n8 n) noexcept {
  m_data += n.m_data;
  return *this;
}

r8 &r8::operator-=(const n8 n) noexcept {
  m_data -= n.m_data;
  return *this;
}

r8 &r8::operator+=(const byte r) noexcept {
  m_data += r;
  return *this;
}

r8 &r8::operator-=(const byte r) noexcept {
  m_data -= r;
  return *this;
}

r8 &r8::operator&=(const r8 r) noexcept {
  m_data &= r.data();
  return *this;
}

r8 &r8::operator&=(const byte b) noexcept {
  m_data &= b;
  return *this;
}

r8 &r8::operator&=(const n8 n) noexcept {
  m_data &= n.m_data;
  return *this;
}

r8 &r8::operator|=(const r8 r) noexcept {
  m_data |= r.data();
  return *this;
}

r8 &r8::operator|=(const byte b) noexcept {
  m_data |= b;
  return *this;
}

r8 &r8::operator|=(const n8 n) noexcept {
  m_data |= n.m_data;
  return *this;
}

r8 &r8::operator^=(const r8 r) noexcept {
  m_data ^= r.data();
  return *this;
}

r8 &r8::operator^=(const byte b) noexcept {
  m_data ^= b;
  return *this;
}

r8 &r8::operator^=(const n8 n) noexcept {
  m_data ^= n.m_data;
  return *this;
}

r8 &r8::operator<<=(const std::uint8_t u) noexcept {
  m_data <<= u;
  return *this;
}

r8 &r8::operator>>=(const std::uint8_t u) noexcept {
  m_data >>= u;
  return *this;
}

byte operator+(const r8 l, const r8 r) noexcept {
  return l.data() + r.data();
}

byte operator+(const r8 r, const byte b) noexcept {
  return r.data() + b;
}

byte operator+(const r8 r, const n8 n) noexcept {
  return r.data() + n.m_data;
}

byte operator-(const r8 l, const r8 r) noexcept {
  return l.data() - r.data();
}

byte operator-(const r8 r, const byte b) noexcept {
  return r.data() - b;
}

byte operator-(const r8 r, const n8 n) noexcept {
  return r.data() - n.m_data;
}

byte operator~(const r8 r) noexcept {
  return ~r.data();
}

}
