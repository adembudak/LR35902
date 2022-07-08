#pragma once

#include <LR35902/config.h>

#include <array>
#include <vector>

namespace LR35902 {

class Cartridge {
private:
  std::vector<byte> m_rom{};
  std::array<byte, 8_KiB> m_sram{};

  enum class mbc : std::uint8_t {
    // clang-format off
    rom_only                       = 0x00,
    mbc1                           = 0x01,
    mbc1_ram                       = 0x02,
    mbc1_ram_battery               = 0x03,
    mbc2                           = 0x05,
    mbc2_battery                   = 0x06,
    rom_ram                        = 0x08,
    rom_ram_battery                = 0x09,
    mmm01                          = 0x0b,
    mmm01_ram                      = 0x0c,
    mmm01_ram_battery              = 0x0d,
    mbc3_timer_battery             = 0x0f,
    mbc3_timer_ram_battery         = 0x10,
    mbc3                           = 0x11,
    mbc3_ram                       = 0x12,
    mbc3_ram_battery               = 0x13,
    mbc5                           = 0x19,
    mbc5_ram                       = 0x1a,
    mbc5_ram_battery               = 0x1b,
    mbc5_rumble                    = 0x1c,
    mbc5_rumble_ram                = 0x1d,
    mbc5_rumble_ram_battery        = 0x1e,
    mbc6                           = 0x20,
    mbc7_sensor_rumble_ram_battery = 0x22,
    pocketCamera                   = 0xfc,
    bandaiTama5                    = 0xfd,
    huc3                           = 0xfe,
    huc1_ram_battery               = 0xff
    // clang-format on
  };

  struct {
    bool nintendo_logo_check{};
    const char title[12]{}; // 11 character + '\0'
    mbc kind{};
    bool checksum{};
    byte rom_size{};
    byte ram_size{};
  } CartridgeHeader;

public:
  void load(const char *romfile);

  byte &operator[](const std::size_t index) noexcept;

  [[nodiscard]] byte readSRAM(const std::size_t index) const noexcept;
  void writeSRAM(const std::size_t index, const byte b) noexcept;

  [[nodiscard]] byte readROM(const std::size_t index) const noexcept;
  void writeROM(const std::size_t index, const byte b) noexcept;

  [[nodiscard]] auto data() const noexcept -> std::vector<byte>;

  friend class DebugView;

private:
  void decodeCartridgeHeader();
};
}
