#pragma once

#include <LR35902/cartridge/kind/mbc1.h>
#include <LR35902/cartridge/kind/rom_only.h>
#include <LR35902/cartridge/kind/rom_ram.h>
#include <LR35902/config.h>

#include <cstdint>
#include <optional>
#include <variant>

namespace LR35902 {
enum class mbc : std::uint8_t;

// this things: https://en.wikipedia.org/wiki/ROM_cartridge#/media/File:PokemonSilverBoard.jpg
class Cartridge {
  std::variant<rom_only, rom_ram, mbc1> m_cart;

  struct {
    bool nintendo_logo_check{};
    char title[12]{}; // 11 character + '\0'
    mbc kind{};
    bool checksum{};
    std::string_view rom_size{};
    std::string_view ram_size{};
  } CartridgeHeader;

public:
  void load(const char *romfile) noexcept;

  [[nodiscard]] byte readROM(const std::size_t index) const noexcept;
  void writeROM(const std::size_t index, const byte b) noexcept;

  [[nodiscard]] byte readSRAM(const std::size_t index) const noexcept;
  void writeSRAM(const std::size_t index, const byte b) noexcept;

  [[nodiscard]] byte &operator[](const std::size_t index) noexcept;
  [[nodiscard]] const byte *data() const noexcept;
  [[nodiscard]] std::size_t size() const noexcept;

  [[nodiscard]] bool hasSRAM() const noexcept;
  [[nodiscard]] std::optional<std::size_t> RAMSize() const noexcept;
  [[nodiscard]] std::optional<const byte *> RAMData() const noexcept;

  friend class DebugView;
};
}
