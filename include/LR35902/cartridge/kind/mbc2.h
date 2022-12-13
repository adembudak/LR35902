#pragma once

#include <LR35902/config.h>

#include <array>
#include <cstdint>
#include <vector>

namespace LR35902 {

class mbc2 final {
  std::vector<byte> m_rom;
  std::array<byte, 512_KiB> m_sram{};

  struct bank_t {
    void setPrimaryBank(const byte b) noexcept;
    [[nodiscard]] std::size_t value() const noexcept;

  private:
    std::size_t primary = 1;
    bool ramg = false;

    std::size_t primary_bank_mask = 0b1111;

    friend class mbc2;
  } bank;

public:
  explicit mbc2(std::vector<byte> rom);

  [[nodiscard]] byte readROM(const std::size_t index) const noexcept;
  void writeROM(const std::size_t index, const byte b) noexcept;

  [[nodiscard]] byte readSRAM(const std::size_t index) const noexcept;
  void writeSRAM(const std::size_t index, const byte b) noexcept;

  friend class Cartridge;
};

}
