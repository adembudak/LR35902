#pragma once

#include "../../config.h"

#include <vector>

namespace LR35902 {

class Cartridge {
private:
  std::vector<byte> m_rom{};

public:
  void load(const char *romfile);

  byte &operator[](const std::size_t index) noexcept;

  [[nodiscard]] byte read(const std::size_t index) const noexcept;
  void write(const std::size_t index, const byte b) noexcept;

  [[nodiscard]] auto data() const noexcept -> std::vector<byte>;

public:
  friend class DebugView;
};
}
