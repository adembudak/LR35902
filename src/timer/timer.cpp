#include <LR35902/interrupt/interrupt.h>
#include <LR35902/io/io.h>
#include <LR35902/timer/timer.h>

#include <cstddef>
#include <cstdint>

namespace LR35902 {

[[nodiscard]] constexpr std::size_t pow(std::size_t base, std::size_t power) noexcept {
  std::size_t result = 1;
  for(std::size_t i = 0; i < power; ++i)
    result *= base;

  return result;
}

// clang-format off
constexpr std::size_t base_clock_in_T_cycles = 4'194'304;

constexpr std::size_t base_clock = base_clock_in_T_cycles / 4;
static_assert(1'048'576 == base_clock); // in M cycles

constexpr std::size_t TAC_0 = base_clock / pow(2, 10); static_assert( 1024 == TAC_0);
constexpr std::size_t TAC_1 = base_clock / pow(2,  4); static_assert(65536 == TAC_1);
constexpr std::size_t TAC_2 = base_clock / pow(2,  6); static_assert(16384 == TAC_2);
constexpr std::size_t TAC_3 = base_clock / pow(2,  8); static_assert( 4096 == TAC_3);

static_assert(base_clock / TAC_0 == 1024);
static_assert(base_clock / TAC_1 ==   16);
static_assert(base_clock / TAC_2 ==   64);
static_assert(base_clock / TAC_3 ==  256);

constexpr std::array<std::size_t, 4>      frequency_select{1024, 16, 64, 256};
constexpr std::size_t div_increase_rate = frequency_select[2];

Timer::Timer(IO &io, Interrupt &intr) :
    m_io{io},
    m_intr{intr} {}

void Timer::update(const std::size_t cycles) noexcept {
  counter += cycles;
  div_counter += cycles;

  if(div_counter > div_increase_rate) {
    div_counter -= div_increase_rate;
    ++m_io.DIV;
  }

  if(const bool isTimerOn = m_io.TAC & 0b0000'0100; isTimerOn) {
    const std::size_t frequency_index = m_io.TAC & 0b0000'0011;
    const std::size_t frequency = frequency_select[frequency_index];

    if(counter > frequency) {
      counter -= frequency;

      if(++m_io.TIMA == 0x00) {                 // 0xff->0x00 timer overflowed
        m_intr.request(Interrupt::kind::timer); // request interrupt
        m_io.TIMA = m_io.TMA;                   // load it to Timar Modulo Accumulator
      }
    }
  }

} // end Timer::update(std::size_t)

} // end namespace LR35902
