#pragma once

#include <LR35902/config.h>

#include <array>
#include <cstdint>

namespace LR35902 {

class rom_only {
  std::array<byte, 32_KiB> m_rom{};

public:
  explicit rom_only() = default;

  explicit rom_only(auto begin, auto end) {
    std::move(begin, end, m_rom.begin());
  }

  [[nodiscard]] byte read(const std::size_t index) const noexcept;
  void write(const std::size_t index, const byte b) noexcept;

  [[nodiscard]] byte &operator[](const std::size_t index) noexcept;
  [[nodiscard]] const byte *data() const noexcept;
};

}
