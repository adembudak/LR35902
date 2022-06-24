#include <LR35902/config.h>
#include <LR35902/stubs/io/io.h>

namespace LR35902 {

[[nodiscard]] byte IO::readIO(const std::size_t index) const noexcept {
  return m_data[index];
}

void IO::writeIO(const std::size_t index, const byte b) noexcept {
  m_data[index] = b;
}

}
