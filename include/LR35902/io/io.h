#pragma once
#include <LR35902/config.h>

#include <array>

namespace LR35902 {

// https://archive.org/details/GameBoyProgManVer1.1/page/n16/mode/1up
class IO {
  std::array<byte, 127_B> m_data{};

public:
  IO() = default;
  // joypad
  byte &P1 = m_data[0x00];

  // serial cable
  byte &SB = m_data[0x01];
  byte &SC = m_data[0x02];

  // timer registers
  byte &DIV = m_data[0x04];
  byte &TIMA = m_data[0x05];
  byte &TMA = m_data[0x06];
  byte &TAC = m_data[0x07];

  // sound registers
  byte &NR10 = m_data[0x10];
  byte &NR11 = m_data[0x11];
  byte &NR12 = m_data[0x12];
  byte &NR13 = m_data[0x13];
  byte &NR14 = m_data[0x14];

  byte &NR21 = m_data[0x16];
  byte &NR22 = m_data[0x17];
  byte &NR23 = m_data[0x18];
  byte &NR24 = m_data[0x19];

  byte &NR30 = m_data[0x1A];
  byte &NR31 = m_data[0x1B];
  byte &NR32 = m_data[0x1C];
  byte &NR33 = m_data[0x1D];
  byte &NR34 = m_data[0x1E];

  byte &NR41 = m_data[0x20];
  byte &NR42 = m_data[0x21];
  byte &NR43 = m_data[0x22];
  byte &NR44 = m_data[0x23];
  byte &NR50 = m_data[0x24];
  byte &NR51 = m_data[0x25];
  byte &NR52 = m_data[0x26];

#if defined(CGB)
  // infrared (CGB only)
  byte &KEY1 = m_data[0x4d];
  byte &RP = m_data[0x56];

  // bank registers (CGB only)
  byte &VBK = m_data[0x4f];
  byte &SVBK = m_data[0x70];
#endif
  // interrupt registers
  byte &IF = m_data[0x0f]; // interrupt request

  // LCD registers
  byte &LCDC = m_data[0x40];
  byte &STAT = m_data[0x41];

  byte &SCY = m_data[0x42];
  byte &SCX = m_data[0x43];

  byte &LY = m_data[0x44];
  byte &LYC = m_data[0x45];

  byte &DMA = m_data[0x46];

  byte &BGP = m_data[0x47];
  byte &OBP0 = m_data[0x48];
  byte &OBP1 = m_data[0x49];

  byte &WY = m_data[0x4a];
  byte &WX = m_data[0x4b];

#if defined(CGB)
  // DMA (CGB only)
  byte &HDMA1 = m_data[0x51];
  byte &HDMA2 = m_data[0x52];
  byte &HDMA3 = m_data[0x53];
  byte &HDMA4 = m_data[0x54];
  byte &HDMA5 = m_data[0x55];

  // Palettes (CBG only)
  byte &BCPS = m_data[0x68];
  byte &BCPD = m_data[0x69];
  byte &OCPS = m_data[0x6a];
  byte &OCPD = m_data[0x6b];
#endif

  IO &get() noexcept {
    return *this;
  }

  [[nodiscard]] byte readIO(const std::size_t index) const noexcept;
  void writeIO(const std::size_t index, const byte b) noexcept;

  [[nodiscard]] auto data() const noexcept -> decltype(m_data);
  void reset() noexcept;

  friend class DebugView;
};

}
