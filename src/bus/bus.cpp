#include <LR35902/bootrom/bootrom.h>
#include <LR35902/builtin/builtin.h>
#include <LR35902/bus/bus.h>
#include <LR35902/cartridge/cartridge.h>
#include <LR35902/config.h>
#include <LR35902/dma/dma.h>
#include <LR35902/interrupt/interrupt.h>
#include <LR35902/io/io.h>
#include <LR35902/joypad/joypad.h>
#include <LR35902/memory_map.h>
#include <LR35902/ppu/ppu.h>

#include <mpark/patterns.hpp>

#include <cassert>
#include <cstddef>

namespace LR35902 {

bootROM bootrom;

Bus::Bus(Cartridge &cart, PPU &ppu, BuiltIn &builtIn, DMA &dma, IO &io, Interrupt &interrupt, Joypad &joypad) :
    m_cart{cart},
    m_ppu{ppu},
    m_builtIn{builtIn},
    m_dma{dma},
    m_io{io},
    m_joypad(joypad),
    interruptHandler{interrupt} {}

byte Bus::read(const std::size_t index) const noexcept {
  using namespace mpark::patterns;

  return match(index)(
      pattern(index >= mmap::rom0 && index < mmap::romx_end) =
          [&] {
            if(bootrom.isBootOnGoing() && index < mmap::header_start) //
              return bootrom.read(index);
            return m_cart.readROM(index);
          },
      pattern(index >= mmap::vram && index < mmap::vram_end) = [&] { return m_ppu.readVRAM(index); },
      pattern(index >= mmap::sram && index < mmap::sram_end) = [&] { return m_cart.readSRAM(index); },
      pattern(index >= mmap::wram0 && index < mmap::wramx_end) = [&] { return m_builtIn.readWRAM(index); },
      pattern(index >= mmap::echo && index < mmap::echo_end) = [&] { return m_builtIn.readEcho(index); },
      pattern(index >= mmap::oam && index < mmap::oam_end) = [&] { return m_ppu.readOAM(index); },
      pattern(index >= mmap::noUse && index < mmap::noUse_end) = [&] { return m_builtIn.readNoUsable(index); },
      pattern(index >= mmap::io &&
              index < mmap::io_end) = [&] { return (index == 0xff00) ? m_joypad.read() : m_io.readIO(index); },
      pattern(index >= mmap::hram && index < mmap::hram_end) = [&] { return m_builtIn.readHRAM(index); },
      pattern(mmap::IE) = [&] { return interruptHandler.IE(); }, //
      pattern(_) = [] { return random_byte(); });
}

void Bus::write(const std::size_t index, const byte b) noexcept {
  using namespace mpark::patterns;

  match(index)(
      pattern(index >= mmap::rom0 && index < mmap::romx_end) = [&] { m_cart.writeROM(index, b); },
      pattern(index >= mmap::vram && index < mmap::vram_end) = [&] { m_ppu.writeVRAM(index, b); },
      pattern(index >= mmap::sram && index < mmap::sram_end) = [&] { m_cart.writeSRAM(index, b); },
      pattern(index >= mmap::wram0 && index < mmap::wramx_end) = [&] { m_builtIn.writeWRAM(index, b); },
      pattern(index >= mmap::echo && index < mmap::echo_end) = [&] { m_builtIn.writeEcho(index, b); },
      pattern(index >= mmap::oam && index < mmap::oam_end) = [&] { m_ppu.writeOAM(index, b); },
      pattern(index >= mmap::noUse && index < mmap::noUse_end) = [&] { m_builtIn.writeNoUsable(index, b); },
      pattern(index >= mmap::io && index < mmap::io_end) =
          [&] {
            match(index)(
                pattern(0xff46) = [&] { m_dma.action(b); }, //
                pattern(0xff50) = [&] { bootrom.unmap(); }, //
                pattern(_) = [&] { m_io.writeIO(index, b); });
          },
      pattern(index >= mmap::hram && index < mmap::hram_end) = [&] { m_builtIn.writeHRAM(index, b); },
      pattern(mmap::IE) = [&] { interruptHandler.IE(b); }, //
      pattern(_) = [] {});
}

// https://gbdev.io/pandocs/Power_Up_Sequence.html#hardware-registers
void Bus::setPostBootValues() noexcept {
  write(0xff00 /* P1 */, 0xcf);
  write(0xff01 /* SB */, 0x00);
  write(0xff02 /* SC */, 0x7e);
  write(0xff04 /* DIV */, 0xab);
  write(0xff05 /* TIMA */, 0x00);
  write(0xff06 /* TMA */, 0x00);
  write(0xff07 /* TAC */, 0xf8);
  write(0xff0f /* IF */, 0xe1);
  write(0xff10 /* NR10 */, 0x80);
  write(0xff11 /* NR11 */, 0xbf);
  write(0xff12 /* NR12 */, 0xf3);
  write(0xff13 /* NR13 */, 0xff);
  write(0xff14 /* NR14 */, 0xbf);
  write(0xff16 /* NR21 */, 0x3f);
  write(0xff17 /* NR22 */, 0x00);
  write(0xff18 /* NR23 */, 0xff);
  write(0xff19 /* NR24 */, 0xbf);
  write(0xff1a /* NR30 */, 0x7f);
  write(0xff1b /* NR31 */, 0xff);
  write(0xff1c /* NR32 */, 0x9f);
  write(0xff1d /* NR33 */, 0xff);
  write(0xff1e /* NR34 */, 0xbf);
  write(0xff20 /* NR41 */, 0xff);
  write(0xff21 /* NR42 */, 0x00);
  write(0xff22 /* NR43 */, 0x00);
  write(0xff23 /* NR44 */, 0xbf);
  write(0xff24 /* NR50 */, 0x77);
  write(0xff25 /* NR51 */, 0xf3);
  write(0xff26 /* NR52 */, 0xf1);
  write(0xff40 /* LCDC */, 0x91);
  write(0xff41 /* STAT */, 0x85);
  write(0xff42 /* SCY */, 0x00);
  write(0xff43 /* SCX */, 0x00);
  write(0xff44 /* LY */, 0x00);
  write(0xff45 /* LYC */, 0x00);
  write(0xff46 /* DMA */, 0xff);
  write(0xff47 /* BGP */, 0xfc);
  write(0xff48 /* OBP0 */, 0x00);
  write(0xff49 /* OBP1 */, 0x00);
  write(0xff4a /* WY */, 0x00);
  write(0xff4b /* WX */, 0x00);
  write(0xff4d /* KEY1 */, 0xff);
  write(0xff4f /* VBK */, 0xff);
  write(0xff51 /* HDMA1 */, 0xff);
  write(0xff52 /* HDMA2 */, 0xff);
  write(0xff53 /* HDMA3 */, 0xff);
  write(0xff54 /* HDMA4 */, 0xff);
  write(0xff55 /* HDMA5 */, 0xff);
  write(0xff56 /* RP */, 0xff);
  write(0xff68 /* BCPS */, 0xff);
  write(0xff69 /* BCPD */, 0xff);
  write(0xff6a /* OCPS */, 0xff);
  write(0xff6b /* OCPD */, 0xff);
  write(0xff70 /* SVBK */, 0xff);
  write(0xffff /* IE */, 0x00);
}

bool Bus::loadBootROM() noexcept {
  return bootrom.load();
}
}
