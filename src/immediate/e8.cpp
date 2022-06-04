#include <LR35902/immediate/e8.h>

namespace LR35902 {

e8 &e8::operator++(int) noexcept {
  ++m_data;
  return *this;
}

const e8 e8::operator++() noexcept {
  const auto temp = *this;
  ++m_data;
  return temp;
}

e8 &e8::operator--(int) noexcept {
  --m_data;
  return *this;
}

const e8 e8::operator--() noexcept {
  const auto temp = *this;
  --m_data;
  return temp;
}

}
