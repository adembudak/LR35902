#include <LR35902/cartridge/cartridge.h>
#include <LR35902/cartridge/header/header.h>
#include <LR35902/cartridge/kind/mbc1_1mb.h>
#include <LR35902/cartridge/kind/mbc2.h>
#include <LR35902/cartridge/kind/mbc5.h>
#include <LR35902/cartridge/kind/rom_only.h>
#include <LR35902/cartridge/kind/rom_ram.h>
#include <LR35902/config.h>
#include <LR35902/memory_map.h>

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

  std::array<byte, mmap::header_end> buf;
  std::copy_n(dumpedGamePak.begin(), mmap::header_end, buf.begin());
  this->header.assign(std::move(buf));

  switch(header.decode_mbc_type().second) {
  case 0x00: m_cart = rom_only(dumpedGamePak); break;
  case 0x01:
    switch(std::size(dumpedGamePak)) {
    case(1_MiB): m_cart = mbc1_1mb(dumpedGamePak); break;
    case(2_MiB): assert(false); break;
    }
    break;
  case 0x02: m_cart = mbc1_ram(dumpedGamePak, header.decode_ram_size().second); break;
  case 0x03: m_cart = mbc1_ram(dumpedGamePak, header.decode_ram_size().second); break;
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
  return std::visit(overloaded { 
                                 [&](const rom_only &rom) { return rom.read(index);    },
                                 [&](const rom_ram &rom)  { return rom.readROM(index); },
                                 [&](const mbc1_1mb &rom) { return rom.readROM(index); },
                                 [&](const mbc1_ram &rom) { return rom.readROM(index); },
                                 [&](const mbc2     &rom) { return rom.readROM(index); },
                                 [&](const mbc5     &rom) { return rom.readROM(index); }
                               }, m_cart);
}

void Cartridge::writeROM(const std::size_t index, const byte b) noexcept {
    std::visit(overloaded { 
                            [&](rom_only &rom) { rom.write(index, b);    },
                            [&](rom_ram &rom)  { rom.writeROM(index, b); },
                            [&](mbc1_1mb &rom) { rom.writeROM(index, b); },
                            [&](mbc1_ram &rom) { rom.writeROM(index, b); },
                            [&](mbc2     &rom) { rom.writeROM(index, b); },
                            [&](mbc5     &rom) { rom.writeROM(index, b); }
                          }, m_cart);
}

byte Cartridge::readSRAM(const std::size_t index) const noexcept {
  return std::visit(overloaded { 
                                 [&](const rom_only &)    { return random_byte();       },
                                 [&](const rom_ram &rom)  { return rom.readSRAM(index); },
                                 [&](const mbc1_1mb &)    { return random_byte();       },
                                 [&](const mbc1_ram &rom) { return rom.readSRAM(index); },
                                 [&](const mbc2     &rom) { return rom.readSRAM(index); },
                                 [&](const mbc5 &)        { return random_byte();       }
                               }, m_cart);
}


void Cartridge::writeSRAM(const std::size_t index, const byte b) noexcept {
  std::visit(overloaded { 
                          [&](rom_only &  )  {    /* do nothing */      }, // no ram in rom only carts
                          [&](rom_ram &rom)  { rom.writeSRAM(index, b); },
                          [&](mbc1_1mb &rom) {    /* do nothing */      } ,
                          [&](mbc1_ram &rom) { rom.writeSRAM(index, b); },
                          [&](mbc2     &rom) { rom.writeSRAM(index, b); },
                          [&](mbc5    &rom ) {    /* do nothing */      },
                        }, m_cart);
}

const byte *Cartridge::data() const noexcept {
  return std::visit(overloaded { 
                                 [&](const rom_only &rom) { return rom.m_rom.data(); },
                                 [&](const rom_ram &rom)  { return rom.m_rom.data(); },
                                 [&](const mbc1_1mb &rom) { return rom.m_rom.data(); },
                                 [&](const mbc1_ram &rom) { return rom.m_rom.data(); },
                                 [&](const mbc2     &rom) { return rom.m_rom.data(); },
                                 [&](const mbc5& rom)     { return rom.m_rom.data(); },
                               }, m_cart);
}

std::size_t Cartridge::size() const noexcept {
  return std::visit(overloaded { 
                                 [&](const rom_only &rom) { return rom.m_rom.size(); },
                                 [&](const rom_ram &rom)  { return rom.m_rom.size(); },
                                 [&](const mbc1_1mb &rom) { return rom.m_rom.size(); },
                                 [&](const mbc1_ram &rom) { return rom.m_rom.size(); },
                                 [&](const mbc2     &rom) { return rom.m_rom.size(); },
                                 [&](const mbc5 &rom)     { return rom.m_rom.size(); },
                               }, m_cart);
}

std::optional<const byte *> Cartridge::SRAMData() const noexcept {
  if(std::holds_alternative<rom_only>(m_cart))      { return std::nullopt;                             }
  else if(std::holds_alternative<rom_ram>(m_cart))  { return std::get<rom_ram>(m_cart).m_sram.data();  }
  else if(std::holds_alternative<mbc1_1mb>(m_cart)) { return std::nullopt;                             }
  else if(std::holds_alternative<mbc1_ram>(m_cart)) { return std::get<mbc1_ram>(m_cart).m_sram.data(); }
  else if(std::holds_alternative<mbc2>(m_cart))     { return std::get<mbc2>(m_cart).m_sram.data();     }
  else if(std::holds_alternative<mbc5>(m_cart))     { return std::nullopt;                             }
  else                                              { return std::nullopt;                             }
}

std::optional<std::size_t> Cartridge::SRAMSize() const noexcept {
  if(std::holds_alternative<rom_only>(m_cart))      { return std::nullopt;                             }
  else if(std::holds_alternative<rom_ram>(m_cart))  { return std::get<rom_ram>(m_cart).m_sram.size();  }
  else if(std::holds_alternative<mbc1_1mb>(m_cart)) { return std::nullopt;                             }
  else if(std::holds_alternative<mbc1_ram>(m_cart)) { return std::get<mbc1_ram>(m_cart).m_sram.size(); }
  else if(std::holds_alternative<mbc2>(m_cart))     { return std::get<mbc2>(m_cart).m_sram.size();     }
  else if(std::holds_alternative<mbc5>(m_cart))     { return std::nullopt;                             }
  else                                              { return std::nullopt;                             }
}


void Cartridge::reset() noexcept { // only resets SRAM
  std::visit(overloaded { 
                          [&](rom_only &)    {      /* no sram */                     },
                          [&](rom_ram &rom)  { std::ranges::fill(rom.m_sram, byte{}); },
                          [&](mbc1_1mb &)    {      /* no sram */                     },
                          [&](mbc1_ram &rom) { std::ranges::fill(rom.m_sram, byte{}); },
                          [&](mbc2 &rom)     { std::ranges::fill(rom.m_sram, byte{}); },
                          [&](mbc5 &)        {      /* no sram */                     }
                        }, m_cart);
}

}
