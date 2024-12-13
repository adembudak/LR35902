#include <LR35902/cartridge/kind/mbc3.h>
#include <LR35902/config.h>
#include <LR35902/memory_map.h>

#include <range/v3/view/chunk.hpp>
#include <range/v3/view/const.hpp>

#include <cassert>
#include <chrono>
#include <cstddef>

// Verbatim implementation of
// https://gbdev.io/pandocs/MBC3.html
// https://archive.org/details/GameBoyProgManVer1.1/page/n219/mode/1up

namespace LR35902 {
namespace rv = ranges::views;

mbc3::mbc3(std::vector<byte> rom, const std::size_t ram_size, const bool has_timer, const bool has_battery) :
    m_rom(std::move(rom)),
    m_sram(ram_size),
    has_timer{has_timer},
    has_battery{has_battery} {
  if(has_timer) {
    using namespace std::chrono;

    const auto now = system_clock::now();
    const auto here_and_now = zoned_time{current_zone(), now};
    const auto local_time_ = here_and_now.get_local_time();
    const auto local_days_ = local_days{floor<std::chrono::days>(local_time_)};
    const auto wall_clock = hh_mm_ss{local_time_ - local_days_};
    const auto ymd = year_month_day{local_days_};
    const auto day_of_year = (local_days_ - local_days{year{ymd.year()} / January / day{0}}).count();

    RTC.seconds = wall_clock.seconds().count();
    RTC.minutes = wall_clock.minutes().count();
    RTC.hours = wall_clock.hours().count();
    RTC.days_lo = day_of_year & 0xff;
    RTC.days_hi = (day_of_year & 0x0100) >> 8;
  } else {
    RTC = {{}, {}, {}, {}, {}};
  }
}

byte mbc3::readROM(const address_t index) const noexcept {
  if(index < mmap::rom0_end) {
    return m_rom[index];
  }

  else if(index < mmap::romx_end) {
    const std::size_t index_normalized = index % rom_bank_size;
    static const auto banked_rom_view = m_rom | rv::const_ | rv::chunk(rom_bank_size);
    return banked_rom_view[ROM_bank][index_normalized];
  }

  else {
    assert(false);
  }
}

void mbc3::writeROM(const address_t index, const byte b) noexcept {
  if(index < 0x2000) {
    RAM_enabled = b == 0x0A;
  }

  else if(index < mmap::rom0_end) {
    ROM_bank = b & 0x7F;
    if(ROM_bank == 0) ++ROM_bank;
  }

  else if(index < 0x6000) {
    SRAM_bank = b;
  }

  else if(index < mmap::romx_end) {
    latch = b; // implement 0->1 logic
  }

  else {
    assert(false);
  }
}

byte mbc3::readSRAM(const address_t index) const noexcept {
  if(RAM_enabled) {
    if(SRAM_bank >= 0 || SRAM_bank <= 3) {
      return m_sram[(SRAM_bank * sram_bank_size) + index];
    }

    else {
      if(latch) {
        switch(SRAM_bank) {
        case 0x08: return RTC.seconds;
        case 0x09: return RTC.minutes;
        case 0x0A: return RTC.hours;
        case 0x0B: return RTC.days_lo;
        case 0x0C: return RTC.days_hi;
        default: break;
        }
      }
    }
  }

  return random_byte();
}

void mbc3::writeSRAM(const address_t index, const byte b) noexcept {
  if(RAM_enabled) {
    if(SRAM_bank >= 0 || SRAM_bank <= 3) {
      m_sram[(SRAM_bank * sram_bank_size) + index] = b;
    }

    else {
      switch(SRAM_bank) {
      case 0x08: RTC.seconds = b & 0x3f; break;
      case 0x09: RTC.minutes = b & 0x3f; break;
      case 0x0A: RTC.hours = b & 0x1f; break;
      case 0x0B: RTC.days_lo = b; break;
      case 0x0C: RTC.days_hi = b & 0xc1; break;
      default: break;
      }
    }
  }
}

}
