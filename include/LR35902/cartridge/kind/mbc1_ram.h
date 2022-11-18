#pragma once

#include <LR35902/config.h>

#include <cstddef>
#include <vector>

namespace LR35902 {

class mbc1_ram {
  std::vector<byte> m_rom;
  std::vector<byte> m_sram;

  struct bank_t {
    void setPrimaryBank(const byte b) noexcept;
    void setSecondaryBank(const byte b) noexcept;
    [[nodiscard]] std::size_t value() const noexcept;

  private:
    std::size_t primary = 1;
    std::size_t secondary = 0;
    bool ramg = false;

    std::size_t primary_bank_mask = 0b1'1111;
    std::size_t secondary_bank_mask = 0b11;

    friend class mbc1_ram;
  } bank;

  enum class mode_t : bool { rom_banking, ram_banking };
  mode_t mode = mode_t::rom_banking;

public:
  mbc1_ram(std::vector<byte> other, const std::size_t ram_size);

  [[nodiscard]] byte readROM(const std::size_t index) const noexcept;
  void writeROM(const std::size_t index, const byte b) noexcept;

  [[nodiscard]] byte readSRAM(const std::size_t index) const noexcept;
  void writeSRAM(const std::size_t index, const byte b) noexcept;

  [[nodiscard]] const byte *data() const noexcept;
  [[nodiscard]] const std::size_t size() const noexcept;
  [[nodiscard]] std::size_t RAMSize() const noexcept;

  void reset() noexcept;

  friend class cartridge;
};

}
