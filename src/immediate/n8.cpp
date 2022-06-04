#include <LR35902/immediate/n8.h>

namespace LR35902 {
n8 &n8::operator++(int) noexcept {
  ++m_data;
  return *this;
}

const n8 n8::operator++() noexcept {
  const auto temp = *this;
  ++m_data;
  return temp;
}

n8 &n8::operator--(int) noexcept {
  --m_data;
  return *this;
}

const n8 n8::operator--() noexcept {
  const auto temp = *this;
  --m_data;
  return temp;
}

}
