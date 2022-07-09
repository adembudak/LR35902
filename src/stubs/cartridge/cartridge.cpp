#include <LR35902/config.h>
#include <LR35902/stubs/cartridge/cartridge.h>
#include <LR35902/stubs/cartridge/kind/rom_only.h>
#include <LR35902/stubs/cartridge/kind/rom_ram.h>

#include <cstdint>
#include <fstream>
#include <vector>

namespace LR35902 {

constexpr byte nintendo_logo[]{0xce, 0xed, 0x66, 0x66, 0xcc, 0x0d, 0x00, 0x0b, 0x03, 0x73, 0x00, 0x83,
                               0x00, 0x0c, 0x00, 0x0d, 0x00, 0x08, 0x11, 0x1f, 0x88, 0x89, 0x00, 0x0e,
                               0xdc, 0xcc, 0x6e, 0xe6, 0xdd, 0xdd, 0xd9, 0x99, 0xbb, 0xbb, 0x67, 0x63,
                               0x6e, 0x0e, 0xec, 0xcc, 0xdd, 0xdc, 0x99, 0x9f, 0xbb, 0xb9, 0x33, 0x3e};

void Cartridge::load(const char *romfile) {
  std::ifstream fin{romfile};
  fin.exceptions(std::ifstream::failbit);

  std::vector<byte> dumpedGamePak(std::istreambuf_iterator<char>{fin}, {});
}

// clang-format off
template <typename... Ts> struct overloaded : Ts... { using Ts::operator()...; };

byte Cartridge::readROM(const std::size_t index) const noexcept {
  return std::visit(overloaded { [&](const rom_only &rom) { return rom.read(index);    },
                                 [&](const rom_ram &rom)  { return rom.readROM(index); }
                               }, m_cart);
}

void Cartridge::writeROM(const std::size_t index, const byte b) noexcept {
  std::visit(overloaded { [&](rom_only &rom) { rom.write(index, b);    },
                          [&](rom_ram &rom)  { rom.writeROM(index, b); }
                        }, m_cart);
}

byte Cartridge::readSRAM(const std::size_t index) const noexcept {
  return std::visit(overloaded { [&](const rom_only &)   { return random_byte();       },
                                 [&](const rom_ram &rom) { return rom.readSRAM(index); }
                               }, m_cart);
}


void Cartridge::writeSRAM(const std::size_t index, const byte b) noexcept {
  std::visit(overloaded { [&](rom_only &  ) {    /* do nothing */      }, // no ram in rom only carts
                          [&](rom_ram &rom) { rom.writeSRAM(index, b); }
                        }, m_cart);
}

byte &Cartridge::operator[](const std::size_t index) noexcept {
  if(std::holds_alternative<rom_only>(m_cart)) { return std::get<rom_only>(m_cart)[index]; }
  else                                         { return std::get<rom_ram>(m_cart)[index];  }
}

auto Cartridge::data() const noexcept -> decltype(auto) {
  return std::visit(overloaded { [&](const rom_only &rom){ return rom.data();  },
                                 [&](const rom_ram &rom) { return rom.data();  }
                               }, m_cart);
}

}
