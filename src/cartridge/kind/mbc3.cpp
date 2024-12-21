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
    const auto here_and_now_ = zoned_time{current_zone(), now};
    const auto local_time_ = here_and_now_.get_local_time();
    const auto local_days_ = local_days{floor<std::chrono::days>(local_time_)};
    const auto wall_clock_ = hh_mm_ss{local_time_ - local_days_};
    const auto ymd_ = year_month_day{local_days_};
    const auto day_of_year_ = (local_days_ - local_days{year{ymd_.year()} / January / day{0}}).count();

    RTC.seconds = wall_clock_.seconds().count();
    RTC.minutes = wall_clock_.minutes().count();
    RTC.hours = wall_clock_.hours().count();
    RTC.days_lo = day_of_year_ & 0xff;
    RTC.days_hi = (day_of_year_ & 0x0100) >> 8;
  } else {
    RTC = {{}, {}, {}, {}, {}};
  }
}

byte mbc3::readROM(address_t index) const noexcept {
  if(index < mmap::rom0_end) {
    return m_rom[index];
  }

  else if(index < mmap::romx_end) {
    index = normalize_index(index, mmap::romx);
    static const auto banked_rom_view = m_rom | rv::const_ | rv::chunk(rom_bank_size);
    return banked_rom_view[ROM_bank][index];
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
    latch = b;
    latch_checker.push_back(b);
    is_latch_open = (latch_checker[0] == 0 && latch_checker[1] == 1); 
  }

  else {
    assert(false);
  }
}

byte mbc3::readSRAM(address_t index) const noexcept {
  index = normalize_index(index, mmap::sram);

  if(RAM_enabled) {
    if(SRAM_bank >= 0 || SRAM_bank <= 3) {
      return m_sram[(SRAM_bank * sram_bank_size) + index];
    }

    else {
      if(is_latch_open) {
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

void mbc3::writeSRAM(address_t index, const byte b) noexcept {
  index = normalize_index(index, mmap::sram);

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
