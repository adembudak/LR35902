#pragma once

#include "../../config.h"
#include "../cartridge/cartridge.h"

#include <array>

namespace LR35902 {
class Bus {
public:
  Bus(const Bus &) = delete;
  Bus &operator=(const Bus &) = delete;

  Bus(Bus &&) = delete;
  Bus &operator=(Bus &&) = delete;

public:
  [[nodiscard]] static Bus &reach() noexcept;
  [[nodiscard]] byte &operator[](const std::size_t i) noexcept;

private:
  Bus() = default;
  Cartridge m_cart;
};
}
