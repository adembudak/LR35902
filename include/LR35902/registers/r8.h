#pragma once

#include "../config.h"

namespace LR35902 {

class r8 {
private:
  byte m_data = 0;

public:
  uint8_t data() const noexcept { return m_data; }
};

} // namespace LR35902
