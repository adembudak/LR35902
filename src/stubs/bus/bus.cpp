#include "LR35902/stubs/bus/bus.h"

namespace LR35902 {

Bus &Bus::reach() noexcept {
  static Bus instance;
  return instance;
}

byte &Bus::operator[](const std::size_t i) noexcept {
  return m_data[i];
}

}
