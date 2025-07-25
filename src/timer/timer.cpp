#include <LR35902/interrupt/interrupt.h>
#include <LR35902/io/io.h>
#include <LR35902/timer/timer.h>

#include <ranges>

#include <cstddef>
#include <cstdint>

namespace LR35902 {

[[nodiscard]] constexpr std::size_t pow(std::size_t base, std::size_t power) noexcept {
  std::size_t result = 1;
  for(auto _ : std::views::iota(0u, power))
    result *= base;

  return result;
}

// clang-format off
constexpr std::size_t base_clock = 1'048'576;

constexpr std::size_t TAC_00_frequency = base_clock / pow(2, 10);
constexpr std::size_t TAC_01_frequency = base_clock / pow(2,  4);
constexpr std::size_t TAC_10_frequency = base_clock / pow(2,  6);
constexpr std::size_t TAC_11_frequency = base_clock / pow(2,  8);

static_assert( 1024 == TAC_00_frequency);
static_assert(65536 == TAC_01_frequency);
static_assert(16384 == TAC_10_frequency);
static_assert( 4096 == TAC_11_frequency);

static_assert((base_clock / TAC_00_frequency) / 4  == 256);
static_assert((base_clock / TAC_01_frequency) / 4 ==   4);
static_assert((base_clock / TAC_10_frequency) / 4 ==   16);
static_assert((base_clock / TAC_11_frequency) / 4 ==  64);

constexpr std::array<std::size_t, 4>      frequency_select{ 256, 4, 16, 64 };
constexpr std::size_t div_increase_rate = frequency_select[2];

// clang-format on

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

    const std::uint16_t bit_to_check = [&] {
      switch(frequency_index) {
      case 0: return 8;
      case 1: return 2;
      case 2: return 4;
      case 3: return 6;
      }
    }();

    byte prev_bit = (previous_counter >> bit_to_check) & 1;
    byte current_bit = (counter >> bit_to_check) & 1;

    if(prev_bit == 1 && current_bit == 0) {
      if(++m_io.TIMA == 0x00) {                 // 0xff->0x00 timer overflowed
        m_intr.request(Interrupt::kind::timer); // request interrupt
        m_io.TIMA = m_io.TMA;                   // load it to Timer Modulo Accumulator
      }
    }
  }

  previous_counter = counter;
}

} // end namespace LR35902
