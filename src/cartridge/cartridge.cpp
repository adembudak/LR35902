#include <LR35902/cartridge/cartridge.h>
#include <LR35902/cartridge/header/header.h>
#include <LR35902/cartridge/kind/all.h>
#include <LR35902/config.h>
#include <LR35902/memory_map.h>

#include <algorithm>
#include <array>
#include <cassert>
#include <cstddef>
#include <fstream>
#include <iterator>
#include <optional>
#include <utility>
#include <variant>
#include <vector>

namespace LR35902 {

void Cartridge::load(const char *romfile) noexcept {
  std::ifstream fin{romfile};
  std::vector<byte> dumpedGamePak(std::istreambuf_iterator<char>{fin}, {});

  std::array<byte, mmap::header_end> buf;
  std::copy_n(dumpedGamePak.begin(), mmap::header_end, buf.begin());
  this->header.assign(std::move(buf));

  switch(header.decode_mbc_type().second) {
  case 0x00: m_cart = rom_only(dumpedGamePak); break;
  case 0x01: m_cart = mbc1(dumpedGamePak, 0, false); break;
  case 0x02: m_cart = mbc1(dumpedGamePak, header.decode_ram_size().second, false); break;
  case 0x03: m_cart = mbc1(dumpedGamePak, header.decode_ram_size().second, true); break;
  case 0x05: m_cart = mbc2(dumpedGamePak); break;
  case 0x06: m_cart = mbc2(dumpedGamePak); break;
  case 0x08: m_cart = rom_ram(begin(dumpedGamePak), end(dumpedGamePak)); break;
  case 0x09: m_cart = rom_ram(begin(dumpedGamePak), end(dumpedGamePak)); break;
  case 0x19: m_cart = mbc5(dumpedGamePak); break;                  /////////////////////
  case 0x0c: /* mmm01_ram                      */ [[fallthrough]]; /////////////////////
  case 0x0d: /* mmm01_ram_battery              */ [[fallthrough]]; /////////////////////
  case 0x0f: /* mbc3_timer_battery             */ [[fallthrough]]; ////  TODO: /////////
  case 0x10: /* mbc3_timer_ram_battery         */ [[fallthrough]]; //// Implement //////
  case 0x11: /* mbc3                           */ [[fallthrough]]; //// these //////////
  case 0x12: /* mbc3_ram                       */ [[fallthrough]]; //// cartridges /////
  case 0x13: /* mbc3_ram_battery               */ [[fallthrough]]; /////////////////////
  case 0x1a: /* mbc5_ram                       */ [[fallthrough]]; /////////////////////
  case 0x1b: /* mbc5_ram_battery               */ [[fallthrough]]; /////////////////////
  case 0x1c: /* mbc5_rumble                    */ [[fallthrough]]; /////////////////////
  case 0x1d: /* mbc5_rumble_ram                */ [[fallthrough]]; /////////////////////
  case 0x1e: /* mbc5_rumble_ram_battery        */ [[fallthrough]]; /////////////////////
  case 0x20: /* mbc6                           */ [[fallthrough]]; /////////////////////
  case 0x22: /* mbc7_sensor_rumble_ram_battery */ [[fallthrough]]; /////////////////////
  case 0xfc: /* pocketCamera                   */ [[fallthrough]]; /////////////////////
  case 0xfd: /* bandaiTama5                    */ [[fallthrough]]; /////////////////////
  case 0xfe: /* huc3                           */ [[fallthrough]]; /////////////////////
  case 0xff: /* huc1_ram_battery               */ [[fallthrough]]; /////////////////////
  default: assert(false && "this type of cart not supported (yet)"); break;
  };
}

// clang-format off
template <typename... Ts> struct overloaded : Ts... { using Ts::operator()...; };
template <typename... Ts> overloaded(Ts...) -> overloaded<Ts...>;

byte Cartridge::readROM(const std::size_t index) const noexcept {
  return std::visit([&](const auto &cart) { return cart.readROM(index); }, m_cart);
}

void Cartridge::writeROM(const std::size_t index, const byte b) noexcept {
    std::visit([&](auto &cart) { return cart.writeROM(index, b); }, m_cart);
}

byte Cartridge::readSRAM(const std::size_t index) const noexcept {
  return std::visit(overloaded { 
                                 [&](const auto &rom)  { return rom.readSRAM(index); },
                                 [&](const rom_only &) { return random_byte();       },
                                 [&](const mbc5 &)     { return random_byte();       }
                               }, m_cart);
}


void Cartridge::writeSRAM(const std::size_t index, const byte b) noexcept {
  std::visit(overloaded { 
                          [&](auto &rom)  { rom.writeSRAM(index, b); },
                          [&](rom_only &) {    /* do nothing */      },
                          [&](mbc5 &)     {    /* do nothing */      },
                        }, m_cart);
}

const byte *Cartridge::data() const noexcept {
  return std::visit([&](const auto &cart) { return cart.m_rom.data(); }, m_cart);
}

std::size_t Cartridge::size() const noexcept {
  return std::visit([&](const auto &cart) { return cart.m_rom.size(); }, m_cart);
}


std::optional<const byte *> Cartridge::SRAMData() const noexcept {
  if(std::holds_alternative<rom_only>(m_cart))        { return std::nullopt;                             }
  else if(std::holds_alternative<rom_ram>(m_cart))    { return std::get<rom_ram>(m_cart).m_sram.data();  }
  else if(std::holds_alternative<mbc1>(m_cart)) { return std::get<mbc1>(m_cart).m_sram.data(); }
  else if(std::holds_alternative<mbc2>(m_cart))       { return std::get<mbc2>(m_cart).m_sram.data();     }
  else if(std::holds_alternative<mbc5>(m_cart))       { return std::nullopt;                             }
  else                                                { return std::nullopt;                             }
}

std::optional<std::size_t> Cartridge::SRAMSize() const noexcept {
  if(std::holds_alternative<rom_only>(m_cart))        { return std::nullopt;                             }
  else if(std::holds_alternative<rom_ram>(m_cart))    { return std::get<rom_ram>(m_cart).m_sram.size();  }
  else if(std::holds_alternative<mbc1>(m_cart))   { return std::get<mbc1>(m_cart).m_sram.size(); }
  else if(std::holds_alternative<mbc2>(m_cart))       { return std::get<mbc2>(m_cart).m_sram.size();     }
  else if(std::holds_alternative<mbc5>(m_cart))       { return std::nullopt;                             }
  else                                                { return std::nullopt;                             }
}


void Cartridge::reset() noexcept { // only resets SRAM
  std::visit(overloaded { 
                          [&](rom_only &)    {      /* no sram */                     },
                          [&](rom_ram &rom)  { std::ranges::fill(rom.m_sram, byte{}); },
                          [&](mbc1 &rom)     { std::ranges::fill(rom.m_sram, byte{}); },
                          [&](mbc2 &rom)     { std::ranges::fill(rom.m_sram, byte{}); },
                          [&](mbc5 &)        {      /* no sram */                     }
                        }, m_cart);
}

}
