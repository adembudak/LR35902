#include <LR35902/cartridge/kind/mbc1.h>
#include <LR35902/cartridge/kind/mbc_config.h>
#include <LR35902/config.h>
#include <LR35902/memory_map.h>

#include <catch2/catch_test_macros.hpp>

#include <cstddef>
#include <utility>

using namespace LR35902;

TEST_CASE("Memory bank controller cartridge type 1", "Read/Write ROM") {
  std::vector<byte> ROM(2_MiB, byte{});

  STATIC_CHECK(2_MiB == 128 * 16_KiB); // 128 ROM banks

  for(int j = 13, i = 0; i < 128; ++i) {
    ROM[i * 16_KiB] = j++;
  }

  REQUIRE(ROM[0 * 16_KiB] == 13);
  REQUIRE(ROM[1 * 16_KiB] == 13 + 1);
  REQUIRE(ROM[2 * 16_KiB] == 13 + 2);
  REQUIRE(ROM[3 * 16_KiB] == 13 + 3);
  REQUIRE(ROM[127 * 16_KiB] == 13 + 127);

  mbc1 cart{std::move(ROM), {.sram_size = 32_KiB}};
  enum : int { reg0 = 0, reg1 = 1, reg2 = 2, reg3 = 3 };

  const auto selectRegister = [&cart](const int reg, const byte val) {
    switch(reg) {
    case 0: cart.writeROM(0x0000, val); break;
    case 1: cart.writeROM(0x2000, val); break;
    case 2: cart.writeROM(0x4000, val); break;
    case 3: cart.writeROM(0x6000, val); break;
    }
  };

  SECTION("ROM operations") {
    REQUIRE(cart.readROM(0) == 13);
    REQUIRE(cart.readROM(1) == 0);
    REQUIRE(cart.readROM(2) == 0);
    REQUIRE(cart.readROM(mmap::romx - 1) == 0);

    selectRegister(reg1, 0);
    REQUIRE(cart.readROM(mmap::romx) == 13 + 1);

    selectRegister(reg1, 1); // switch to romx 1
    REQUIRE(cart.readROM(mmap::romx) == 13 + 1);

    selectRegister(reg1, 2);
    REQUIRE(cart.readROM(mmap::romx) == 13 + 2);

    selectRegister(reg1, 3);
    REQUIRE(cart.readROM(mmap::romx) == 13 + 3);

    selectRegister(reg1, 31);
    REQUIRE(cart.readROM(mmap::romx) == 13 + 31);

    STATIC_CHECK(32 == 0b10'0000);
    selectRegister(reg1, 32);
    REQUIRE(cart.readROM(mmap::romx) != 13 + 32);
    REQUIRE(cart.readROM(mmap::romx) == 13 + 1);

    selectRegister(reg1, 1); // set register 1 to 1, lower rom bank number
    selectRegister(reg2, 1); // set register 2 to 1, upper rom bank number
    selectRegister(reg3, 0); // set register 3 to 0, enable upper rom banking
    STATIC_CHECK(((1 << 5) | 1) == 33);
    REQUIRE(cart.readROM(mmap::romx) == 13 + 33);

    selectRegister(reg1, 2);
    selectRegister(reg2, 1);
    REQUIRE(cart.readROM(mmap::romx) == 13 + 34);

    STATIC_CHECK(((3 << 5) | 31) == 127);
    selectRegister(reg1, 31);
    selectRegister(reg2, 3);
    REQUIRE(cart.readROM(mmap::romx) == 13 + 127);

    selectRegister(reg3, 1); // set register 3 to 1, disable upper rom banking
    REQUIRE(cart.readROM(mmap::romx) == 13 + 31);
  }

  SECTION("RAM operations") {
    selectRegister(reg3, 0); // disable ram banking
    selectRegister(reg0, 0); // disable ram gate register
    cart.writeSRAM(0, 123);  // white should have no effect, read should return random value
    REQUIRE_FALSE(cart.readSRAM(0) == 123);

    selectRegister(reg0, 0x0a); // enable ram gate register
    cart.writeSRAM(0, 123);
    REQUIRE(cart.readSRAM(0) == 123);

    selectRegister(reg3, 1); // enable ram banking
    selectRegister(reg2, 0); // read from bank 0
    REQUIRE(cart.readSRAM(0) == 123);

    selectRegister(reg2, 1);
    cart.writeSRAM(0, 0xab);
    REQUIRE(cart.readSRAM(0) == 0xab);

    selectRegister(reg2, 2);
    cart.writeSRAM(0, 0xbc);
    REQUIRE(cart.readSRAM(0) == 0xbc);

    selectRegister(reg2, 3);
    cart.writeSRAM(0, 0xcd);
    REQUIRE(cart.readSRAM(0) == 0xcd);

    STATIC_CHECK(4 == 0b100);
    STATIC_CHECK((0b100 & 0x03) == 00);
    selectRegister(reg2, 4);
    REQUIRE(cart.readSRAM(0) == 123);
  }
}
