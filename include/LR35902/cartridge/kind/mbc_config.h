#pragma once

#include <cstddef>

namespace LR35902 {
struct MBC_config {
  std::size_t sram_size;
  bool has_timer;
  bool has_battery;
  bool has_rumble;
};
}