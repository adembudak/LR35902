#pragma once

#include <LR35902/cartridge/header/header.h>
#include <LR35902/cartridge/kind/all.h>
#include <LR35902/config.h>

#include <cstdint>
#include <optional>
#include <variant>

namespace LR35902 {

// this things: https://en.wikipedia.org/wiki/ROM_cartridge#/media/File:PokemonSilverBoard.jpg
class Cartridge final {
  header_t header;
  using cart_t = std::variant<rom_only, rom_ram, mbc1, mbc2, mbc5>;
  cart_t m_cart;

public:
  void load(const char *const romfile) noexcept;

  [[nodiscard]] byte readROM(const std::size_t index) const noexcept;
  void writeROM(const std::size_t index, const byte b) noexcept;

  [[nodiscard]] byte readSRAM(const std::size_t index) const noexcept;
  void writeSRAM(const std::size_t index, const byte b) noexcept;

  [[nodiscard]] const byte *data() const noexcept;
  [[nodiscard]] std::size_t size() const noexcept;

  [[nodiscard]] std::optional<const byte *> SRAMData() const noexcept;
  [[nodiscard]] std::optional<std::size_t> SRAMSize() const noexcept;

  void reset() noexcept;

  friend class DebugView;
};
}
