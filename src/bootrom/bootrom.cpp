#include <LR35902/bootrom/bootrom.h>
#include <LR35902/config.h>

#include <cassert>
#include <fstream>

namespace LR35902 {

void bootROM::load() noexcept {
  std::ifstream fin{"bootrom.gb"};

  assert(fin.is_open() && "boot rom (bootrom.gb) not found\n");

  m_data.assign(std::istreambuf_iterator<char>{fin}, {});
  m_data.resize(0x100);
}

void bootROM::unmap() noexcept {
  m_data.clear();
}

bool bootROM::isBootOnGoing() const noexcept {
  return m_data.empty() == false;
}

byte &bootROM::read(std::size_t index) noexcept {
  return m_data[index];
}

}
