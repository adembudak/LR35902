#include <LR35902/cartridge/kind/mbc2/mbc2.h>
#include <LR35902/config.h>
#include <LR35902/memory_map.h>

#include <catch2/catch_test_macros.hpp>

#include <cstddef>
#include <utility>

using namespace LR35902;

TEST_CASE("Memory bank controller type 2", "ROM banking behavior") {
  std::vector<byte> ROM(256_KiB, byte{});

  STATIC_CHECK(rom_bank_size == 16_KiB);
  STATIC_CHECK(256_KiB == 16 * rom_bank_size); // 16 ROM banks;

  for(byte j = 34, i = 0; i < 16; ++i) {
    ROM[i * 16_KiB] = j++;
  }

  REQUIRE(ROM[0 * 16_KiB] == 34);
  REQUIRE(ROM[1 * 16_KiB] == 34 + 1);
  REQUIRE(ROM[2 * 16_KiB] == 34 + 2);
  REQUIRE(ROM[3 * 16_KiB] == 34 + 3);
  REQUIRE(ROM[15 * 16_KiB] == 34 + 15);

  mbc2 cart{std::move(ROM)};

  SECTION("ROM Operations") {
    REQUIRE(cart.readROM(0) == 34);

    cart.writeROM(0x0100, 0); // selecting bank 0 should default to 1
    REQUIRE(cart.readROM(mmap::romx) == 34 + 1);

    cart.writeROM(0x0100, 1);
    REQUIRE(cart.readROM(mmap::romx) == 34 + 1);

    cart.writeROM(0x0100, 2);
    REQUIRE(cart.readROM(mmap::romx) == 34 + 2);

    cart.writeROM(0x0100, 15);
    REQUIRE(cart.readROM(mmap::romx) == 34 + 15);
  }

  SECTION("RAM Operations") {
    cart.writeROM(0x0000, 0x0A); // ram enabled

    cart.writeSRAM(mmap::sram, 0xab); // writing should discard upper nibble of the byte
    REQUIRE(cart.readSRAM(mmap::sram) == 0x0b);

    cart.writeSRAM(mmap::sram + 3, 0x12);
    for(int i = 0; i < 16; ++i) // testing "echo" behavior
      REQUIRE(cart.readSRAM(mmap::sram + 3 + (i * 512_B)) == 0x02);

    cart.writeROM(0x0000, 0x00);                // ram disabled
    cart.writeSRAM(mmap::sram, 0x0c);           // write should has no effect
    REQUIRE(cart.readSRAM(mmap::sram) != 0x0c); // read should return a random value
  }
}
