#include <LR35902/cartridge/kind/mbc5.h>
#include <LR35902/config.h>
#include <LR35902/memory_map.h>

#include <catch2/catch_test_macros.hpp>

#include <cstddef>
#include <utility>

using namespace LR35902;

namespace {

[[nodiscard]] consteval unsigned long long operator"" _ROMBANK(const unsigned long long l) noexcept {
  return rom_bank_size * l;
}

[[nodiscard]] consteval unsigned long long operator"" _SRAMBANK(const unsigned long long l) noexcept {
  return sram_bank_size * l;
}

}

TEST_CASE("Memory bank controller cartridge type 5", "Read/Write ROM") {

  std::vector<byte> ROM(512_ROMBANK, byte{});

  STATIC_CHECK(512_ROMBANK == 8_MiB); // Available banks [0, 512)

  // Populate the rom with some data
  for(std::size_t j = 0, i = 0; i < 256; ++i) {
    ROM[i * 16_KiB] = j++;
  }
  ROM[1] = 33;
  REQUIRE(ROM[0_ROMBANK + 1] == 33);

  for(std::size_t j = 255, i = 256; i < 512; ++i) {
    ROM[i * 16_KiB] = j--;
  }

  REQUIRE(ROM[0_ROMBANK] == 0);
  REQUIRE(ROM[1_ROMBANK] == 1);
  REQUIRE(ROM[1_ROMBANK + 1] == 0);
  REQUIRE(ROM[255_ROMBANK] == 255);

  REQUIRE(ROM[256_ROMBANK] == 255);
  REQUIRE(ROM[257_ROMBANK] == 254);
  REQUIRE(ROM[511_ROMBANK] == 0);

  mbc5 cart{std::move(ROM), 16_SRAMBANK, false, false};

  STATIC_CHECK(16_SRAMBANK == 128_KiB); // [0, 16)

  enum { ramg = 0, romb_0, romb_1, ramb } regs{};

  const auto set_register = [&cart](const int reg, const byte val) {
    switch(reg) {
    case ramg: cart.writeROM(0x0000, val); break;
    case romb_0: cart.writeROM(0x2000, val); break;
    case romb_1: cart.writeROM(0x3000, val); break;
    case ramb: cart.writeROM(0x4000, val);  break;
    }
  };

  SECTION("ROM operations") {
    REQUIRE(cart.readROM(0) == 0);

    cart.writeROM(0x2000, 1); // switch to bank 1
    REQUIRE(cart.readROM(mmap::romx) == 1);

    cart.writeROM(0x2000, 2);
    REQUIRE(cart.readROM(mmap::romx) == 2);

    cart.writeROM(0x2000, 255);
    REQUIRE(cart.readROM(mmap::romx) == 255);

    set_register(romb_0, 0);
    set_register(romb_1, 1);
    REQUIRE(cart.readROM(mmap::romx) == 255);

    set_register(romb_0, 1);
    set_register(romb_1, 1);
    REQUIRE(cart.readROM(mmap::romx) == 254);

    set_register(romb_0, 254);
    set_register(romb_1, 1);
    REQUIRE(cart.readROM(mmap::romx) == 1);
 
    set_register(romb_0, 255);
    set_register(romb_1, 1);
    REQUIRE(cart.readROM(mmap::romx) == 0);
  }

  SECTION("RAM operations") {
   set_register(ramg, 0); // ram disabled, write has no effect and read returns random
   
   set_register(ramb, 0);  // ram bank 0 selected
   cart.writeSRAM(mmap::sram, 33);
   REQUIRE(cart.readSRAM(mmap::sram) != 33);

   set_register(ramg, 0x0a); // ram enabled
   
   cart.writeSRAM(mmap::sram, 33);
   REQUIRE(cart.readSRAM(mmap::sram) == 33);

   cart.writeSRAM(mmap::sram, 44);
   REQUIRE(cart.readSRAM(mmap::sram) == 44);


   set_register(ramb, 1);

   cart.writeSRAM(mmap::sram + 3, 33);
   REQUIRE(cart.readSRAM(mmap::sram + 3) == 33);

   set_register(ramb, 15);

   cart.writeSRAM(mmap::sram + 4, 44);
   REQUIRE(cart.readSRAM(mmap::sram + 4) != 55);
   REQUIRE(cart.readSRAM(mmap::sram + 4) == 44);
  }
}
