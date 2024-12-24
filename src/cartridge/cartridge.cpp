#include <LR35902/cartridge/cartridge.h>
#include <LR35902/cartridge/header/header.h>
#include <LR35902/cartridge/kind/all.h>
#include <LR35902/cartridge/kind/mbc_config.h>
#include <LR35902/config.h>
#include <LR35902/memory_map.h>

#include <mpark/patterns/match.hpp>

#include <algorithm>
#include <array>
#include <cassert>
#include <cstddef>
#include <fstream>
#include <iterator>
#include <optional>
#include <string>
#include <utility>
#include <variant>
#include <vector>

namespace LR35902 {

bool Cartridge::load(const char *const romfile) noexcept {
  std::ifstream fin{romfile};
  if(!fin) return false;

  std::vector<byte> dumpedGamePak(std::istreambuf_iterator<char>{fin}, {});
  dumpedGamePak.shrink_to_fit();

  if(std::size(dumpedGamePak) == 0) return false;

  std::array<byte, mmap::header_end> buf;
  std::copy_n(dumpedGamePak.begin(), mmap::header_end, buf.begin());
  this->header.assign(std::move(buf));

  using namespace mpark::patterns;
  const auto RAM_size = header.decode_ram_size().second;
  match(header.decode_mbc_type().second)(
      pattern(0x00) = [&] { m_cart = rom_only(dumpedGamePak); },
      pattern(0x01) = [&] { m_cart = mbc1(dumpedGamePak, {}); },
      pattern(0x02) = [&] { m_cart = mbc1(dumpedGamePak, {.sram_size = RAM_size}); },
      pattern(0x03) = [&] { m_cart = mbc1(dumpedGamePak, {.sram_size = RAM_size, .has_battery = true}); },
      pattern(0x05) = [&] { m_cart = mbc2(dumpedGamePak, {}); },
      pattern(0x06) = [&] { m_cart = mbc2(dumpedGamePak, {.has_battery = true}); },
      pattern(0x08) = [&] { m_cart = rom_ram(dumpedGamePak, {}); },
      pattern(0x09) = [&] { m_cart = rom_ram(dumpedGamePak, { .has_battery = true }); },
      pattern(0x0f) = [&] { m_cart = mbc3(dumpedGamePak, {.has_timer = true, .has_battery = true}); },
      pattern(0x10) = [&] { m_cart = mbc3(dumpedGamePak, {.sram_size = RAM_size, .has_timer = true, .has_battery = true}); },
      pattern(0x11) = [&] { m_cart = mbc3(dumpedGamePak, {}); },
      pattern(0x12) = [&] { m_cart = mbc3(dumpedGamePak, {.sram_size = RAM_size}); },
      pattern(0x13) = [&] { m_cart = mbc3(dumpedGamePak, {.sram_size = RAM_size, .has_battery = true}); },
      pattern(0x19) = [&] { m_cart = mbc5(dumpedGamePak, {}); },
      pattern(0x1a) = [&] { m_cart = mbc5(dumpedGamePak, {.sram_size = RAM_size}); },
      pattern(0x1b) = [&] { m_cart = mbc5(dumpedGamePak, {.sram_size = RAM_size, .has_battery = true}); },
      pattern(0x1c) = [&] { m_cart = mbc5(dumpedGamePak, {.has_rumble = true}); },
      pattern(0x1d) = [&] { m_cart = mbc5(dumpedGamePak, {.sram_size = RAM_size, .has_rumble = true}); },
      pattern(0x1e) = [&] { m_cart = mbc5(dumpedGamePak, {.sram_size = RAM_size, .has_battery = true, .has_rumble = true}); },
      pattern(_) =
          [&] {
            const std::string msg =                       //
                "this type of cart not suppported (yet) " //
                + header.decode_mbc_type().first + " "    //
                + std::to_string(header.decode_mbc_type().second);
            assert(false && msg.c_str());
          });

  return true;
}

// clang-format off
template <typename... Ts> struct overloaded : Ts... { using Ts::operator()...; };
template <typename... Ts> overloaded(Ts...) -> overloaded<Ts...>;

byte Cartridge::readROM(const address_t index) const noexcept {
  return std::visit([&](const auto &cart) { return cart.readROM(index); }, m_cart);
}

void Cartridge::writeROM(const address_t index, const byte b) noexcept {
    std::visit([&](auto &cart) { return cart.writeROM(index, b); }, m_cart);
}

byte Cartridge::readSRAM(const address_t index) const noexcept {
  return std::visit(overloaded {
                                 [&](const auto &rom)  { return rom.readSRAM(index); },
                                 [&](const rom_only &) { return random_byte();       },
                               }, m_cart);
}


void Cartridge::writeSRAM(const address_t index, const byte b) noexcept {
  std::visit(overloaded {
                          [&](auto &rom)  { rom.writeSRAM(index, b); },
                          [&](rom_only &) {    /* do nothing */      },
                        }, m_cart);
}

void Cartridge::reset() noexcept { // only resets SRAM
  std::visit(overloaded {
                          [&](auto &rom)  { std::ranges::fill(rom.m_sram, byte{}); },
                          [&](rom_only &) {      /* no sram */                     },
                        }, m_cart);
}

#if defined(WITH_DEBUGGER)
const byte *Cartridge::data() const noexcept {
  return std::visit([&](const auto &cart) { return cart.m_rom.data(); }, m_cart);
}

std::size_t Cartridge::size() const noexcept {
  return std::visit([&](const auto &cart) { return cart.m_rom.size(); }, m_cart);
}


std::optional<const byte *> Cartridge::SRAMData() const noexcept {
  if(std::holds_alternative<rom_only>(m_cart))     { return std::nullopt;                             }
  else if(std::holds_alternative<rom_ram>(m_cart)) { return std::get<rom_ram>(m_cart).m_sram.data();  }
  else if(std::holds_alternative<mbc1>(m_cart))    { return std::get<mbc1>(m_cart).m_sram.data(); }
  else if(std::holds_alternative<mbc2>(m_cart))    { return std::get<mbc2>(m_cart).m_sram.data();     }
  else if(std::holds_alternative<mbc5>(m_cart))    { return std::nullopt;                             }
  else                                             { return std::nullopt;                             }
}

std::size_t Cartridge::SRAMSize() const noexcept{
      return std::visit(overloaded {
                      [&](const auto &rom)  { return std::size(rom.m_sram); },
                      [&](const rom_only &) { return std::size_t{0};        },
                    }, m_cart);
}
#endif
}
