#include <LR35902/cartridge/cartridge.h>
#include <LR35902/cartridge/kind/mbc1.h>
#include <LR35902/cartridge/kind/rom_only.h>
#include <LR35902/cartridge/kind/rom_ram.h>
#include <LR35902/config.h>

#include <algorithm>
#include <cassert>
#include <cstdint>
#include <fstream>
#include <optional>
#include <vector>

namespace LR35902 {

constexpr byte nintendo_logo[]{0xce, 0xed, 0x66, 0x66, 0xcc, 0x0d, 0x00, 0x0b, 0x03, 0x73, 0x00, 0x83,
                               0x00, 0x0c, 0x00, 0x0d, 0x00, 0x08, 0x11, 0x1f, 0x88, 0x89, 0x00, 0x0e,
                               0xdc, 0xcc, 0x6e, 0xe6, 0xdd, 0xdd, 0xd9, 0x99, 0xbb, 0xbb, 0x67, 0x63,
                               0x6e, 0x0e, 0xec, 0xcc, 0xdd, 0xdc, 0x99, 0x9f, 0xbb, 0xb9, 0x33, 0x3e};

enum class mbc : std::uint8_t {
  // clang-format off
    rom_only                       = 0x00, // done
    mbc1                           = 0x01, // done
    mbc1_ram                       = 0x02,
    mbc1_ram_battery               = 0x03,
    mbc2                           = 0x05,
    mbc2_battery                   = 0x06,
    rom_ram                        = 0x08, // done
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

void Cartridge::load(const char *romfile) noexcept {
  std::ifstream fin{romfile};

  const std::vector<byte> dumpedGamePak(std::istreambuf_iterator<char>{fin}, {});

  // https://gbdev.io/pandocs/The_Cartridge_Header.html
  const std::size_t nintendo_logo_begin = 0x104;
  CartridgeHeader.nintendo_logo_check = std::equal(std::cbegin(nintendo_logo), std::cend(nintendo_logo),
                                                   dumpedGamePak.cbegin() + nintendo_logo_begin);

  const std::size_t title_begin = 0x134;
  std::copy_n(dumpedGamePak.begin() + title_begin, 11_B, std::begin(CartridgeHeader.title));

  const std::size_t cartridge_type = 0x147;
  switch(dumpedGamePak[cartridge_type]) {
  case 0x00:
    CartridgeHeader.kind = mbc::rom_only;
    m_cart = rom_only(begin(dumpedGamePak), end(dumpedGamePak));
    break;

  case 0x01:
    CartridgeHeader.kind = mbc::mbc1;
    m_cart = mbc1(dumpedGamePak);
    break;

  case 0x08:
    CartridgeHeader.kind = mbc::rom_ram;
    m_cart = rom_ram(begin(dumpedGamePak), end(dumpedGamePak));
    break;

  default: assert(false && "this type of cart not supported (yet)");
  };

  const std::size_t checksum_begin = 0x134;
  const std::size_t checksum_end = 0x14c + 1; // +1 for to use the address as an end iterator
  const byte checksum_result = dumpedGamePak[0x14d];

  CartridgeHeader.checksum =
      checksum_result == std::accumulate(&dumpedGamePak[checksum_begin], &dumpedGamePak[checksum_end], 0,
                                         [](const byte x, const byte y) { return x - y - 1; });

  const byte rom_size = dumpedGamePak[0x0148];
  CartridgeHeader.rom_size = [&rom_size]() -> std::string_view {
    switch(rom_size) {
    case 0x00: return "32_KiB";
    case 0x01: return "64_KiB";
    case 0x02: return "128_KiB";
    case 0x03: return "256_KiB";
    case 0x04: return "512_KiB";
    case 0x05: return "1_MiB";
    case 0x06: return "2_MiB";
    case 0x07: return "4_MiB";
    case 0x08: return "8_MiB";
    case 0x52: return "1.1_MiB";
    case 0x53: return "1.2_MiB";
    case 0x54: return "1.5_MiB";
    default: return "Failed to decode";
    };
  }();

  const byte ram_size = dumpedGamePak[0x0149];
  CartridgeHeader.ram_size = [&ram_size]() -> std::string_view {
    switch(ram_size) {
    case 0x00: return "0_KiB";
    case 0x02: return "8_KiB";
    case 0x03: return "32_KiB";
    case 0x04: return "128_KiB";
    case 0x05: return "64_KiB";
    default: return "Failed to decode";
    }
  }();
}

// clang-format off
template <typename... Ts> struct overloaded : Ts... { using Ts::operator()...; };
template <typename... Ts> overloaded(Ts...) -> overloaded<Ts...>;

byte Cartridge::readROM(const std::size_t index) const noexcept {
  return std::visit(overloaded { [&](const rom_only &rom) { return rom.read(index);    },
                                 [&](const rom_ram &rom)  { return rom.readROM(index); },
                                 [&](const mbc1 &rom)     { return rom.read(index); }
                               }, m_cart);
}

void Cartridge::writeROM(const std::size_t index, const byte b) noexcept {
  std::visit(overloaded { [&](rom_only &rom) { rom.write(index, b);    },
                          [&](rom_ram &rom)  { rom.writeROM(index, b); },
                          [&](mbc1 &rom)     { rom.write(index, b); }
                        }, m_cart);
}

byte Cartridge::readSRAM(const std::size_t index) const noexcept {
  return std::visit(overloaded { [&](const rom_only &)   { return random_byte();       },
                                 [&](const rom_ram &rom) { return rom.readSRAM(index); },
                                 [&](const mbc1 &)       { return random_byte(); }
                               }, m_cart);
}


void Cartridge::writeSRAM(const std::size_t index, const byte b) noexcept {
  std::visit(overloaded { [&](rom_only &  ) {    /* do nothing */      }, // no ram in rom only carts
                          [&](rom_ram &rom) { rom.writeSRAM(index, b); },
                          [&](mbc1 & )      {    /* do nothing */      }
                        }, m_cart);
}

byte &Cartridge::operator[](const std::size_t index) noexcept {
  if(std::holds_alternative<rom_only>(m_cart))     { return std::get<rom_only>(m_cart)[index]; }
  else if(std::holds_alternative<rom_ram>(m_cart)) { return std::get<rom_ram>(m_cart)[index]; }
  else                                             { return std::get<mbc1>(m_cart)[index];  }
}

const byte *Cartridge::data() const noexcept {
  return std::visit(overloaded { [&](const rom_only &rom) { return rom.data();  },
                                 [&](const rom_ram &rom)  { return rom.data();  },
                                 [&](const mbc1 &rom)     { return rom.data();  }
                               }, m_cart);
}

std::size_t Cartridge::size() const noexcept {
  return std::visit(overloaded { [&](const rom_only &rom) { return rom.size();  },
                                 [&](const rom_ram &rom)  { return rom.size();  },
                                 [&](const mbc1 &rom)     { return rom.size();  }
                               }, m_cart);
}


bool Cartridge::hasSRAM() const noexcept {
  return std::visit(overloaded { [&](const rom_only &) { return false;  },
                                 [&](const rom_ram &)  { return true;   },
                                 [&](const mbc1 &)     { return false;  }
                               }, m_cart);
}

std::optional<std::size_t> Cartridge::RAMSize() const noexcept {
  if(std::holds_alternative<rom_only>(m_cart))     { return std::nullopt;                        }
  else if(std::holds_alternative<rom_ram>(m_cart)) { return std::get<rom_ram>(m_cart).RAMSize(); }
  else                                             { return std::nullopt;                        }
}

std::optional<const byte *> Cartridge::RAMData() const noexcept {
  if(std::holds_alternative<rom_only>(m_cart))     { return std::nullopt;                        }
  else if(std::holds_alternative<rom_ram>(m_cart)) { return std::get<rom_ram>(m_cart).RAMData(); }
  else                                             { return std::nullopt;                        }
}

void Cartridge::reset() noexcept { // only resets SRAM
  std::visit(overloaded { [&](rom_only &)    {                     },
                          [&](rom_ram &rom)  { return rom.reset(); },
                          [&](mbc1 &)        {                     }
                        }, m_cart);
}

}
