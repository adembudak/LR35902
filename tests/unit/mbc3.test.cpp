#include <LR35902/cartridge/kind/mbc3.h>
#include <LR35902/config.h>
#include <LR35902/memory_map.h>

#include <catch2/catch_test_macros.hpp>

#include <ranges>

using namespace LR35902;

TEST_CASE("Memory bank controller cartridge type 3", "Read/Write ROM") {
  std::vector<byte> ROM(128_ROMBANK, byte{0xff});
  STATIC_CHECK(2_MiB == 128_ROMBANK); // [0, 128)

  for(size_t i : std::views::iota(0, 128))
    ROM[i * 16_KiB] = i;

  REQUIRE(ROM[0_ROMBANK] == 0);
  REQUIRE(ROM[1_ROMBANK] == 1);
  REQUIRE(ROM[127_ROMBANK] == 127);

  STATIC_CHECK(32_KiB == 4_SRAMBANK); // [0, 4)
  mbc3 cart{ROM, 4_SRAMBANK, true, false};
  enum {
    register_0 = 0, // SRAM enable
    register_1,     // ROM bank select
    register_2,     // SRAM bank select
    register_3      // clock related???
  } regs{};

  const auto set_register = [&cart](const int reg, const byte val) {
    switch(reg) {
    case register_0: cart.writeROM(0x0000, val); break;
    case register_1: cart.writeROM(0x2000, val); break;
    case register_2: cart.writeROM(0x4000, val); break;
    case register_3: cart.writeROM(0x6000, val); break;
    }
  };

  SECTION("ROM operations") {
    REQUIRE(cart.readROM(0) == 0);
    REQUIRE(cart.readROM(1) == 0xff);

    set_register(register_1, 0); // selecting rom 0 defaults bank 1
    REQUIRE(cart.readROM(mmap::romx) == 1);

    set_register(register_1, 1);
    REQUIRE(cart.readROM(mmap::romx) == 1);
    REQUIRE(cart.readROM(mmap::romx + 1) == 0xff);

    set_register(register_1, 127);
    REQUIRE(cart.readROM(mmap::romx) == 127);
    REQUIRE(cart.readROM(mmap::romx + 1) == 0xff);
  }

  SECTION("SRAM operations") {
    cart.writeSRAM(mmap::sram, 0xab); // SRAM disabled by default, write should no effect and read should return a random
    REQUIRE_FALSE(cart.readSRAM(mmap::sram) == 0xab);
    
    set_register(register_0, 0x0a); // should be enabled now
    cart.writeSRAM(mmap::sram, 0xab);
    REQUIRE(cart.readSRAM(mmap::sram) == 0xab);

    set_register(register_2, 1);
    cart.writeSRAM(mmap::sram, 0xbc);
    REQUIRE(cart.readSRAM(mmap::sram) == 0xbc);

     set_register(register_2, 2);
    cart.writeSRAM(mmap::sram, 0xac);
    REQUIRE(cart.readSRAM(mmap::sram) == 0xac);

    set_register(register_2, 3);
    cart.writeSRAM(mmap::sram, 0xcd);
    REQUIRE(cart.readSRAM(mmap::sram) == 0xcd);
  }
}