#include <LR35902/stubs/cartridge/cartridge.h>
#include <LR35902/stubs/cartridge/kind/rom_only.h>
#include <LR35902/stubs/cartridge/kind/rom_ram.h>

#include <cstdint>
#include <fstream>
#include <vector>

namespace LR35902 {

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
