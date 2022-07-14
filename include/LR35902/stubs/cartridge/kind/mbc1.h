#pragma once

#include <LR35902/config.h>

#include <cstdint>
#include <optional>
#include <vector>

namespace LR35902 {

class mbc1 {
public:
  static constexpr bool has_sram = false;

private:
  std::vector<byte> m_rom;

  // registers are zero by default;
  mutable std::size_t primary_bank = 0x0;          // 5 bit register, but depends on cart size
  std::optional<std::size_t> secondary_bank = 0x0; // 2 bit register, only exist when carts size >= 512 kb

  bool secondary_bank_enabled = false;
  std::size_t bank_normalize_mask;

public:
  explicit mbc1(const std::vector<byte> rom);

  [[nodiscard]] byte read(std::size_t index) const noexcept;
  void write(const std::size_t index, const byte b) noexcept;

  [[nodiscard]] byte &operator[](const std::size_t index) noexcept;
  [[nodiscard]] const byte *data() const noexcept;
};

}
