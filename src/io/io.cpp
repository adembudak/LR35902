#include <LR35902/config.h>
#include <LR35902/io/io.h>
#include <LR35902/memory_map.h>

namespace LR35902 {

[[nodiscard]] byte IO::readIO(address_t index) const noexcept {
  index = normalize_index(index, mmap::io);
  return m_data[index];
}

void IO::writeIO(address_t index, const byte b) noexcept {
  index = normalize_index(index, mmap::io);

  if(index == 0x04) { // writes to DIV resets it to 0
    DIV = 0x00;
    return;
  }

  if(index == 0x44) { // LY is read-only
    return;
  }

  m_data[index] = b;
}

auto IO::data() const noexcept -> decltype(m_data) {
  return m_data;
}

void IO::reset() noexcept {
  m_data.fill(byte{});
}

}
