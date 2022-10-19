#include <LR35902/cartridge/cartridge.h>
#include <LR35902/cartridge/kind/mbc1.h>
#include <LR35902/cartridge/kind/rom_only.h>
#include <LR35902/cartridge/kind/rom_ram.h>
#include <LR35902/config.h>

#include <cassert>
#include <cstdint>
#include <fstream>
#include <optional>
#include <vector>

namespace LR35902 {

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

  switch(const std::size_t cartridge_type = 0x147; dumpedGamePak[cartridge_type]) {
  case 0x00: m_cart = rom_only(dumpedGamePak); break;
  case 0x01: m_cart = mbc1(dumpedGamePak); break;
  case 0x08: m_cart = rom_ram(begin(dumpedGamePak), end(dumpedGamePak)); break;
  default: assert(false && "this type of cart not supported (yet)"); break;
  };
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
