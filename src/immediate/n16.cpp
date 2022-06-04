#include <LR35902/immediate/n16.h>

namespace LR35902 {
n16 &n16::operator++(int) noexcept {
  ++m_data;
  return *this;
}

const n16 n16::operator++() noexcept {
  const auto temp = *this;
  ++m_data;
  return temp;
}

n16 &n16::operator--(int) noexcept {
  --m_data;
  return *this;
}

const n16 n16::operator--() noexcept {
  const auto temp = *this;
  --m_data;
  return temp;
}

byte &operator*(const n16 nn) noexcept {
  return Bus::reach()[nn.m_data];
}

}
