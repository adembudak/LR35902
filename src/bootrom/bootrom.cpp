#include <LR35902/bootrom/bootrom.h>
#include <LR35902/config.h>
#include <LR35902/memory_map.h>

#include <cassert>
#include <cstddef>
#include <fstream>

namespace LR35902 {


bool BootROM::load() noexcept {
  std::ifstream fin{"bootrom.gb", std::ios::binary};
  if(!fin) return false;
constexpr std::size_t bootrom_size = mmap::bootrom_end - mmap::bootrom_start;
static_assert(bootrom_size == 0x100); // Boot ROM mapped to first 256 bytes, executed and then unmapped

  m_data.assign(std::istreambuf_iterator<char>{fin}, {});
  m_data.resize(bootrom_size);
  return true;
}

byte BootROM::read(const std::size_t index) const noexcept {
  return m_data[index];
}

bool BootROM::isBootOnGoing() const noexcept {
  return m_data.size() != 0;
}

void BootROM::unmap() noexcept {
  m_data.clear();
  m_data.shrink_to_fit();
}
}
