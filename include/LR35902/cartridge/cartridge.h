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
  using cart_t = std::variant<rom_only, rom_ram, mbc1, mbc2, mbc3, mbc5>;
  cart_t m_cart;

  bool is_bootROM_successfully_loaded = false;
  std::vector<byte> bootrom_buf;
  std::vector<byte> dumpedGamePak;

public:

  bool loadBootROM() noexcept;
  bool loadROM(const char *const romfile) noexcept;

  void mapBootROM() noexcept;
  void unmapBootROM() noexcept;

  [[nodiscard]] byte readROM(const address_t index) const noexcept;
  void writeROM(const address_t index, const byte b) noexcept;

  [[nodiscard]] byte readSRAM(const address_t index) const noexcept;
  void writeSRAM(const address_t index, const byte b) noexcept;

  void reset() noexcept;

  [[nodiscard]] const byte *data() const noexcept;
  [[nodiscard]] std::size_t size() const noexcept;

  [[nodiscard]] std::optional<const byte *> SRAMData() const noexcept;
  [[nodiscard]] std::size_t SRAMSize() const noexcept;

  friend class DebugView;
};
}
