#pragma once

#include <LR35902/config.h>

#include <cstddef>
#include <vector>
#include <array>

namespace LR35902 {

struct MBC_config;

template <typename T, std::size_t N>
struct CircularBuffer { // Poor man's circular buffer implementation
  static_assert(N > 0);

  std::size_t m_index = 0;
  std::array<T, N> m_data{};

  void push_back(const T val) {
    m_data[m_index % N] = val;
    ++m_index;
  }
  T operator[](const std::size_t ndx) const {
    return m_data[ndx];
  }
};

template <std::size_t N>
using circularByteBuffer = CircularBuffer<byte, N>;

class mbc3 final {
  std::vector<byte> m_rom;
  std::vector<byte> m_sram;

  bool has_timer;
  bool has_battery;

  // registers
  byte SRAM_enabled = 0;
  byte ROM_bank = 1;
  byte SRAM_bank = 0;
  byte latch = 0;

  struct RTC_t {
    byte seconds;
    byte minutes;
    byte hours;
    byte days_lo;
    byte days_hi;
  };

  RTC_t RTC;

  circularByteBuffer<2> latch_checker;
  bool is_latch_open = false;

  void update_RTC() noexcept;

public:
  mbc3(std::vector<byte> rom, const MBC_config& config);

  [[nodiscard]] byte readROM(address_t index) const noexcept;
  void writeROM(const address_t index, const byte b) noexcept;

  [[nodiscard]] byte readSRAM(address_t index) const noexcept;
  void writeSRAM(address_t index, const byte b) noexcept;

  friend class Cartridge;
};

}
