#include <LR35902/config.h>
#include <LR35902/io/io.h>
#include <LR35902/memory_map.h>

namespace LR35902 {

[[nodiscard]] byte IO::readIO(const std::size_t index) const noexcept {
  return m_data[index];
}

void IO::writeIO(const std::size_t index, const byte b) noexcept {
  if(index == 0x04) { // writes to DIV resets it to 0
    DIV = 0x00;
    return;
  }

  m_data[index] = b;
}

byte &IO::operator[](const std::size_t index) noexcept {
  return m_data[index - io];
}

auto IO::data() const noexcept -> decltype(m_data) {
  return m_data;
}

void IO::reset() noexcept {
  m_data.fill(byte{});
}

}
