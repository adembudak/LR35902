#include <LR35902/cartridge/cartridge.h>
#include <LR35902/cartridge/header/header.h>
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

void Cartridge::load(const char *romfile) noexcept {
  std::ifstream fin{romfile};
  std::vector<byte> dumpedGamePak(std::istreambuf_iterator<char>{fin}, {});

  std::array<byte, cartridge_header_end> buf;
  std::copy_n(dumpedGamePak.begin(), cartridge_header_end, buf.begin());
  this->header.assign(std::move(buf));

  switch(header.decode_mbc_type().second) {
  case 0x00: m_cart = std::move(rom_only(dumpedGamePak)); break;
  case 0x01: m_cart = std::move(mbc1(dumpedGamePak)); break;
  case 0x02: m_cart = std::move(mbc1_ram(dumpedGamePak, header.decode_ram_size().second)); break;
  case 0x03: m_cart = std::move(mbc1_ram(dumpedGamePak, header.decode_ram_size().second)); break;
  case 0x05: /* mbc2 */ break;
  case 0x06: /* mbc2_battery */ break;
  case 0x08: m_cart = std::move(rom_ram(begin(dumpedGamePak), end(dumpedGamePak))); break;
  case 0x09: m_cart = std::move(rom_ram(begin(dumpedGamePak), end(dumpedGamePak))); break;
  case 0x0c: /* mmm01_ram                      */ [[fallthrough]]; /////////////////////
  case 0x0d: /* mmm01_ram_battery              */ [[fallthrough]]; /////////////////////
  case 0x0f: /* mbc3_timer_battery             */ [[fallthrough]]; ////  TODO: /////////
  case 0x10: /* mbc3_timer_ram_battery         */ [[fallthrough]]; //// Implement //////
  case 0x11: /* mbc3                           */ [[fallthrough]]; //// these //////////
  case 0x12: /* mbc3_ram                       */ [[fallthrough]]; //// cartridges /////
  case 0x13: /* mbc3_ram_battery               */ [[fallthrough]]; /////////////////////
  case 0x19: /* mbc5                           */ [[fallthrough]]; /////////////////////
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
  return std::visit(overloaded { 
                                 [&](const rom_only &rom) { return rom.read(index);    },
                                 [&](const rom_ram &rom)  { return rom.readROM(index); },
                                 [&](const mbc1     &rom) { return rom.readROM(index); },
                                 [&](const mbc1_ram &rom) { return rom.readROM(index); }
                               }, m_cart);
}

void Cartridge::writeROM(const std::size_t index, const byte b) noexcept {
    std::visit(overloaded { 
                            [&](rom_only &rom) { rom.write(index, b);    },
                            [&](rom_ram &rom)  { rom.writeROM(index, b); },
                            [&](mbc1     &rom)     { rom.writeROM(index, b); },
                            [&](mbc1_ram &rom)     { rom.writeROM(index, b); }
                          }, m_cart);
}

byte Cartridge::readSRAM(const std::size_t index) const noexcept {
  return std::visit(overloaded { 
                                 [&](const rom_only &)    { return random_byte();       },
                                 [&](const rom_ram &rom)  { return rom.readSRAM(index); },
                                 [&](const mbc1 &rom)     { return random_byte();       },
                                 [&](const mbc1_ram &rom) { return rom.readSRAM(index); }
                               }, m_cart);
}


void Cartridge::writeSRAM(const std::size_t index, const byte b) noexcept {
  std::visit(overloaded { 
                          [&](rom_only &  ) {    /* do nothing */      }, // no ram in rom only carts
                          [&](rom_ram &rom) { rom.writeSRAM(index, b); },
                          [&](mbc1    &rom )   {    /* do nothing */      } ,
                          [&](mbc1_ram &rom )  { rom.writeSRAM(index, b); }
                        }, m_cart);
}

const byte *Cartridge::data() const noexcept {
  return std::visit(overloaded { 
                                 [&](const rom_only &rom) { return rom.data(); },
                                 [&](const rom_ram &rom)  { return rom.data(); },
                                 [&](const mbc1&rom )     { return rom.data(); },
                                 [&](const mbc1_ram &rom) { return rom.data(); }
                               }, m_cart);
}

std::size_t Cartridge::size() const noexcept {
  return std::visit(overloaded { 
                                 [&](const rom_only &rom) { return rom.size(); },
                                 [&](const rom_ram &rom)  { return rom.size(); },
                                 [&](const mbc1 &rom)     { return rom.size(); },
                                 [&](const mbc1_ram &rom) { return rom.size(); }
                               }, m_cart);
}


bool Cartridge::hasSRAM() const noexcept {
  return std::visit(overloaded { 
                                 [&](const rom_only &) { return false; },
                                 [&](const rom_ram &)  { return true;  },
                                 [&](const mbc1& )     { return false; },
                                 [&](const mbc1_ram &)        { return true; }
                               }, m_cart);
}

std::optional<std::size_t> Cartridge::RAMSize() const noexcept {
  if(std::holds_alternative<rom_only>(m_cart))      { return std::nullopt;                           }
  else if(std::holds_alternative<rom_ram>(m_cart))  { return std::get<rom_ram>(m_cart).RAMSize(); }
  else if(std::holds_alternative<mbc1>(m_cart))     { return std::nullopt;                           }  
  else if(std::holds_alternative<mbc1_ram>(m_cart)) { return std::get<mbc1_ram>(m_cart).RAMSize();   }  
  else                                                 { return std::nullopt;                           }
}

std::optional<const byte *> Cartridge::RAMData() const noexcept {
  if(std::holds_alternative<rom_only>(m_cart))     { return std::nullopt;                           }
  else if(std::holds_alternative<rom_ram>(m_cart)) { return std::get<rom_ram>(m_cart).RAMData(); }
  else if(std::holds_alternative<mbc1_ram>(m_cart))    { return std::nullopt;                           }  
  else                                             { return std::nullopt;                           }
}

void Cartridge::reset() noexcept { // only resets SRAM
  std::visit(overloaded { 
                          [&](rom_only &)    {              },
                          [&](rom_ram &rom)  { rom.reset(); },
                          [&](mbc1  &)       {              },
                          [&](mbc1_ram &rom) { rom.reset(); }
                        }, m_cart);
}

}
