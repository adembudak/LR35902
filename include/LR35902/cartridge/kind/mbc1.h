#pragma once

#include <LR35902/config.h>

#include <cstddef>
#include <vector>

namespace LR35902 {

class mbc1 {
  std::vector<byte> m_rom;

  struct bank_t {
    void setPrimaryBank(const byte b) noexcept;
    void setSecondaryBank(const byte b) noexcept;
    [[nodiscard]] std::size_t value() const noexcept;

  private:
    std::size_t primary = 1;
    std::size_t secondary = 0;

    std::size_t primary_bank_mask = 0b1'1111;
    std::size_t secondary_bank_mask = 0b11;

    friend class mbc1;
  } bank;

public:
  mbc1(std::vector<byte> other);

  [[nodiscard]] byte readROM(const std::size_t index) const noexcept;
  void writeROM(const std::size_t index, const byte b) noexcept;

  friend class Cartridge;
};

}
