#include <LR35902/config.h>
#include <LR35902/registers/r16.h>
#include <LR35902/registers/r8.h>

#include <cstdint>

namespace LR35902 {

std::uint16_t r16::data() const noexcept {
  return std::uint16_t(m_hi.data() << 8 | m_lo.data());
}

byte &operator*(const r16 rr) noexcept {
  return Bus::reach()[rr.data()];
}

}
