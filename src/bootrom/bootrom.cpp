#include <LR35902/bootrom/bootrom.h>
#include <LR35902/config.h>

#include <cassert>
#include <cstddef>
#include <fstream>

namespace LR35902 {

constexpr std::size_t bootrom_size = 0x100;

bool BootROM::load() noexcept {
  std::ifstream fin{"bootrom.gb", std::ios::binary};
  if(!fin) return false;

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
