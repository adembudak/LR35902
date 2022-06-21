#include <LR35902/config.h>
#include <LR35902/registers/r8.h>

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

}
