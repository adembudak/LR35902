#pragma once

#include "../../config.h"

#include <array>

namespace LR35902 {

class Cartridge;

class Bus {
private:
  Cartridge &m_cart;

public:
  [[nodiscard]] explicit Bus(Cartridge &cart);

  [[nodiscard]] byte &read_write(const std::size_t index) noexcept;
  [[nodiscard]] byte read(const std::size_t index) const noexcept;
  void write(const std::size_t index, const byte b) noexcept;
};
}
