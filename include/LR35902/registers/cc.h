#pragma once

namespace LR35902 {

// clang-format off
enum class cc_t {
  z  = 1 << 0,  // Execute if z is set.
  nz = 1 << 1,  // Execute if z is not set.
  c  = 1 << 2,  // Execute if c is set.
  nc = 1 << 3,  // Execute if c is not set.
};

static_assert(1 << 3 == 0b1000);

} // namespace LR35902
