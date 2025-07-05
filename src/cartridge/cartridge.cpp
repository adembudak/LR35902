#include <LR35902/cartridge/cartridge.h>
#include <LR35902/cartridge/header/header.h>
#include <LR35902/cartridge/kind/all.h>
#include <LR35902/cartridge/kind/mbc_config.h>
#include <LR35902/config.h>
#include <LR35902/memory_map.h>

#include <range/v3/algorithm/swap_ranges.hpp>
#include <range/v3/range/conversion.hpp>
#include <range/v3/view/filter.hpp>

#include <mpark/patterns/match.hpp>

#include <algorithm>
#include <array>
#include <cassert>
#include <cstddef>
#include <filesystem>
#include <fstream>
#include <iterator>
#include <optional>
#include <string>
#include <utility>
#include <variant>
#include <vector>

namespace LR35902 {

namespace mp = mpark::patterns;
namespace rg = ranges;
namespace rv = rg::views;
namespace fs = std::filesystem;

bool Cartridge::loadBootROM() noexcept {
  constexpr std::array<std::string_view, 4> possibleBootROMFileNames{"bootrom.gb", "bootrom.bin", "dmg_boot.bin",
                                                                     "dmg0_boot.bin"};
  const auto existingOnes = possibleBootROMFileNames                                        //
                            | rv::filter([](const auto &path) { return fs::exists(path); }) //
                            | rg::to<std::vector<fs::path>>;

  if(std::empty(existingOnes)) return false;

  for(std::ifstream fin; const auto &bootROM : existingOnes) {
    fin.open(bootROM, std::ios::binary);
    if(fin.good()) {
      bootrom_buf.assign(std::istreambuf_iterator<char>{fin}, {});
      break;
    }
  }

  if(std::empty(bootrom_buf)) return false;

  constexpr std::size_t bootrom_size = mmap::bootrom_end - mmap::bootrom_start;
  static_assert(bootrom_size == 0x100); // Boot ROM mapped to first 256 bytes of the memory, executed and then unmapped

  bootrom_buf.resize(bootrom_size);
  bootrom_buf.shrink_to_fit();
  this->is_bootROM_successfully_loaded = true;

  return true;
}

bool Cartridge::loadROM(const char *const romfile) noexcept {
  std::ifstream fin{romfile, std::ios::binary};
  if(!fin) return false;

  dumpedGamePak = std::vector<byte>(std::istreambuf_iterator<char>{fin}, {});
  if(std::size(dumpedGamePak) == 0) return false;

  if(is_bootROM_successfully_loaded) mapBootROM();

  std::array<byte, mmap::header_end> buf;
  std::copy_n(dumpedGamePak.begin(), mmap::header_end, buf.begin());
  this->header.assign(std::move(buf));

  if(!header.is_logocheck_ok()) return false;

  const auto SRAM_size = header.decode_ram_size().second;
  const auto MBC_type = header.decode_mbc_type().second;
  using namespace mp;
  match(MBC_type)(
      pattern(0x00) = [&] { m_cart = rom_only(dumpedGamePak); }, pattern(0x01) = [&] { m_cart = mbc1(dumpedGamePak, {}); },
      pattern(0x02) = [&] { m_cart = mbc1(dumpedGamePak, {.sram_size = SRAM_size}); },
      pattern(0x03) = [&] { m_cart = mbc1(dumpedGamePak, {.sram_size = SRAM_size, .has_battery = true}); },
      pattern(0x05) = [&] { m_cart = mbc2(dumpedGamePak, {}); },
      pattern(0x06) = [&] { m_cart = mbc2(dumpedGamePak, {.has_battery = true}); },
      pattern(0x08) = [&] { m_cart = rom_ram(dumpedGamePak, {}); },
      pattern(0x09) = [&] { m_cart = rom_ram(dumpedGamePak, {.has_battery = true}); },
      pattern(0x0f) = [&] { m_cart = mbc3(dumpedGamePak, {.has_timer = true, .has_battery = true}); },
      pattern(0x10) =
          [&] { m_cart = mbc3(dumpedGamePak, {.sram_size = SRAM_size, .has_timer = true, .has_battery = true}); },
      pattern(0x11) = [&] { m_cart = mbc3(dumpedGamePak, {}); },
      pattern(0x12) = [&] { m_cart = mbc3(dumpedGamePak, {.sram_size = SRAM_size}); },
      pattern(0x13) = [&] { m_cart = mbc3(dumpedGamePak, {.sram_size = SRAM_size, .has_battery = true}); },
      pattern(0x19) = [&] { m_cart = mbc5(dumpedGamePak, {}); },
      pattern(0x1a) = [&] { m_cart = mbc5(dumpedGamePak, {.sram_size = SRAM_size}); },
      pattern(0x1b) = [&] { m_cart = mbc5(dumpedGamePak, {.sram_size = SRAM_size, .has_battery = true}); },
      pattern(0x1c) = [&] { m_cart = mbc5(dumpedGamePak, {.has_rumble = true}); },
      pattern(0x1d) = [&] { m_cart = mbc5(dumpedGamePak, {.sram_size = SRAM_size, .has_rumble = true}); },
      pattern(0x1e) =
          [&] { m_cart = mbc5(dumpedGamePak, {.sram_size = SRAM_size, .has_battery = true, .has_rumble = true}); },
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

void Cartridge::mapBootROM() noexcept {
  rg::swap_ranges(bootrom_buf, dumpedGamePak);
}

void Cartridge::unmapBootROM() noexcept {
  if(is_bootROM_successfully_loaded) {
    std::visit([&](auto &cart) { rg::swap_ranges(bootrom_buf, cart.m_rom); }, m_cart);
    bootrom_buf.resize(0);
    bootrom_buf.shrink_to_fit();
  }
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
                                 [&](const auto &cart)  { return cart.readSRAM(index); },
                                 [&](const rom_only &) { return random_byte();       },
                               }, m_cart);
}

void Cartridge::writeSRAM(const address_t index, const byte b) noexcept {
  std::visit(overloaded {
                          [&](auto &cart)  { cart.writeSRAM(index, b); },
                          [&](rom_only &) {    /* do nothing */      },
                        }, m_cart);
}

void Cartridge::reset() noexcept { // only resets SRAM
  std::visit(overloaded {
                          [&](auto &cart)  { std::ranges::fill(cart.m_sram, byte{}); },
                          [&](rom_only &) {      /* no sram */                     },
                        }, m_cart);
}

const byte *Cartridge::data() const noexcept {
  return std::visit([&](const auto &cart) { return cart.m_rom.data(); }, m_cart);
}

std::size_t Cartridge::size() const noexcept {
  return std::visit([&](const auto &cart) { return cart.m_rom.size(); }, m_cart);
}

std::optional<const byte *> Cartridge::SRAMData() const noexcept {
  if(std::holds_alternative<rom_ram>(m_cart))   { return std::get<rom_ram>(m_cart).m_sram.data(); }
  else if(std::holds_alternative<mbc1>(m_cart)) { return std::get<mbc1>(m_cart).m_sram.data();    }
  else if(std::holds_alternative<mbc2>(m_cart)) { return std::get<mbc2>(m_cart).m_sram.data();    }
  else if(std::holds_alternative<mbc3>(m_cart)) { return std::get<mbc3>(m_cart).m_sram.data();    }
  else if(std::holds_alternative<mbc5>(m_cart)) { return std::get<mbc5>(m_cart).m_sram.data();    }
  else                                          { return std::nullopt;                            }
}

std::size_t Cartridge::SRAMSize() const noexcept{
  return std::visit(overloaded {
                    [&](const auto &cart) { return std::size(cart.m_sram); },
                    [&](const rom_only &) { return std::size_t{0};         } }, m_cart);
}
}
