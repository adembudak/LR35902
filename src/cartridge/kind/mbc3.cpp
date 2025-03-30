#include <LR35902/cartridge/kind/mbc3.h>
#include <LR35902/cartridge/kind/mbc_config.h>
#include <LR35902/config.h>
#include <LR35902/memory_map.h>

// clang-format off
#include <mpark/patterns/match.hpp>
#include <mpark/patterns/when.hpp>
#include <mpark/patterns/anyof.hpp>
// clang-format on

#include <range/v3/view/chunk.hpp>
#include <range/v3/view/const.hpp>

#include <cassert>
#include <chrono>
#include <cstddef>

#if defined(__cpp_lib_chrono) && __cpp_lib_chrono >= 201907L
  #define CHRONO_HAS_TIME_ZONES 1
#else
  #include <date/date.h>
  #include <date/tz.h>
#endif

// Verbatim implementation of
// https://gbdev.io/pandocs/MBC3.html
// https://archive.org/details/GameBoyProgManVer1.1/page/n219/mode/1up

namespace LR35902 {
namespace rv = ranges::views;
namespace mp = mpark::patterns;

void mbc3::update_RTC() noexcept {
  if(const bool is_halted = 0b0100'0000 & RTC.days_hi; is_halted) return;
  using namespace std::chrono;

#if defined(CHRONO_HAS_TIME_ZONES)
  const auto now_ = system_clock::now();
  const auto here_and_now_ = zoned_time{current_zone(), now_};
  const auto local_time_ = here_and_now_.get_local_time();
  const auto local_days_ = local_days{floor<std::chrono::days>(local_time_)};
  const auto wall_clock_ = hh_mm_ss{local_time_ - local_days_};
  const auto ymd_ = year_month_day{local_days_};
  const auto day_of_year_ = (local_days_ - local_days{year{ymd_.year()} / January / day{0}}).count();
#else
  const auto now_ = system_clock::now();
  const auto here_and_now_ = date::zoned_time{date::current_zone(), now_};
  const auto local_time_ = here_and_now_.get_local_time();
  const auto local_days_ = date::local_days{floor<std::chrono::days>(local_time_)};
  const auto wall_clock_ = date::hh_mm_ss{local_time_ - local_days_};
  const auto ymd_ = date::year_month_day{local_days_};
  const auto day_of_year_ = (local_days_ - date::local_days{date::year{ymd_.year()} / date::January / date::day{0}}).count();

#endif

  RTC.seconds = wall_clock_.seconds().count();
  RTC.minutes = wall_clock_.minutes().count();
  RTC.hours = wall_clock_.hours().count();
  RTC.days_lo = day_of_year_ & 0xff;
  RTC.days_hi = (day_of_year_ & 0x0100) >> 8;
}

mbc3::mbc3(std::vector<byte> rom, const MBC_config &config) :
    m_rom(std::move(rom)),
    m_sram(config.sram_size),
    has_timer{config.has_timer},
    has_battery{config.has_battery} {
  if(has_timer) update_RTC();
  else RTC = {{}, {}, {}, {}, {}};
}

// clang-format off
byte mbc3::readROM(address_t index) const noexcept {
  using namespace mp;
  static const auto banked_rom_view = m_rom | rv::const_ | rv::chunk(rom_bank_size);

  return match(index)(
      pattern(_).when(_ >= 0x0000     && _ < mmap::romx) = [&] { return m_rom[index]; },
      pattern(_).when(_ >= mmap::romx && _ < mmap::romx_end) = [&] {
            index = normalize_index(index, mmap::romx);
            return banked_rom_view[ROM_bank][index];
      }
  );
}

void mbc3::writeROM(const address_t index, const byte b) noexcept {
  using namespace mp;
  match(index)(
      pattern(_).when(_ >= 0x0000 && _ < 0x2000) = [&] { SRAM_enabled = b == 0x0A; },
      pattern(_).when(_ >= 0x2000 && _ < 0x4000) = [&] { ROM_bank = b & 0x7F; if(ROM_bank == 0) ++ROM_bank; },
      pattern(_).when(_ >= 0x4000 && _ < 0x6000) = [&] { SRAM_bank = b; },
      pattern(_).when(_ >= 0x6000 && _ < 0x8000) = [&] {
            latch = b;
            latch_checker.push_back(b);
            is_latch_open = (latch_checker[0] == 0 && latch_checker[1] == 1);
      }
  );
}

byte mbc3::readSRAM(address_t index) const noexcept {
  using namespace mp;
  index = normalize_index(index, mmap::sram);
  const_cast<mbc3 *>(this)->update_RTC();

  return match(SRAM_enabled, has_timer, is_latch_open, SRAM_bank) (
      pattern(true, _ , _, anyof(0x00, 0x01, 0x02, 0x03)) = [&] { return m_sram[(SRAM_bank * sram_bank_size) + index]; },
      pattern(true, true, true, 0x08) = [&] { return RTC.seconds; },
      pattern(true, true, true, 0x09) = [&] { return RTC.minutes; },
      pattern(true, true, true, 0x0A) = [&] { return RTC.hours; },
      pattern(true, true, true, 0x0B) = [&] { return RTC.days_lo; },
      pattern(true, true, true, 0x0C) = [&] { return RTC.days_hi; },
      pattern(true, false, _, _) = [] { return random_byte(); },
      pattern(false, _, _, _) = [] { return random_byte(); });
}

void mbc3::writeSRAM(address_t index, const byte b) noexcept {
  using namespace mp;

  index = normalize_index(index, mmap::sram);

  match(SRAM_enabled, has_timer, SRAM_bank)(
      pattern(true, _, anyof(0x00, 0x01, 0x02, 0x03)) = [&] { m_sram[(SRAM_bank * sram_bank_size) + index] = b; },
      pattern(true, true, 0x08) = [&] { RTC.seconds = b & 0x3f; },
      pattern(true, true, 0x09) = [&] { RTC.minutes = b & 0x3f; },
      pattern(true, true, 0x0A) = [&] { RTC.hours = b & 0x1f; },
      pattern(true, true, 0x0B) = [&] { RTC.days_lo = b; },
      pattern(true, true, 0x0C) = [&] { RTC.days_hi = b & 0xc1; },
      pattern(true, _, _) = [] {},
      pattern(false, _, _) = [] {});
}
// clang-format on
}
