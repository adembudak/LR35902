#include <LR35902/config.h>
#include <LR35902/stubs/io/io.h>

namespace LR35902 {

[[nodiscard]] byte IO::readIO(const std::size_t index) const noexcept {
  return m_data[index];
}

void IO::writeIO(const std::size_t index, const byte b) noexcept {
  m_data[index] = b;
}

byte &IO::operator[](const std::size_t index) noexcept {
  return m_data[index];
}

auto IO::data() const noexcept -> decltype(m_data) {
  return m_data;
}

}
