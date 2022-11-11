#pragma once

#include <LR35902/config.h>

#include <cstddef>
#include <vector>

namespace LR35902 {

class mbc1 {
  std::vector<byte> m_rom;

  struct {
    void setPrimaryBank(const byte b) noexcept {
      primary = b & 0b1'1111;

      if(primary == 0) ++primary; // added, can't select rom0
    }

    void setSecondaryBank(const byte b) noexcept {
      secondary = b & 0b11;
    }

    std::size_t value() const noexcept {
      return (secondary << 5) | primary;
    }

  private:
    std::size_t primary = 1;
    std::size_t secondary = 0;
  } bank;

public:
  mbc1(std::vector<byte> another);

  [[nodiscard]] byte readROM(const std::size_t index) const noexcept;
  void writeROM(const std::size_t index, const byte b) noexcept;

  [[nodiscard]] byte readRAM(const std::size_t index) const noexcept;
  void writeRAM(const std::size_t index, const byte b) noexcept;

  [[nodiscard]] const byte *data() const noexcept;
  [[nodiscard]] const std::size_t size() const noexcept;

  friend class cartridge;
};

}
