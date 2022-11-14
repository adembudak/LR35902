#include <LR35902/builtin/builtin.h>
#include <LR35902/cartridge/cartridge.h>
#include <LR35902/cpu/clock/clock.h>
#include <LR35902/dma/dma.h>
#include <LR35902/memory_map.h>
#include <LR35902/ppu/ppu.h>

#include <algorithm>
#include <cassert>
#include <cstddef>

namespace LR35902 {

DMA::DMA(Cartridge &cart, PPU &ppu, BuiltIn &builtIn, Clock &clock) :
    m_cart{cart},
    m_ppu{ppu},
    m_builtIn{builtIn},
    m_clock{clock} {}

constexpr std::size_t amount = 0xa0; // 40 * 32 bit == 160 byte

void DMA::action(const byte n) noexcept {

  if(const std::size_t destination = n * 0x100; destination < romx_end)
    for(std::size_t i = 0; i < amount; ++i)
      m_ppu.m_oam[i] = m_cart.readROM(i);

  else if(destination < vram_end) //
    std::ranges::copy_n(m_ppu.m_vram.begin(), amount, m_ppu.m_oam.begin());

  else if(destination < sram_end)
    for(std::size_t i = 0; i < amount; ++i)
      m_ppu.m_oam[i] = m_cart.readSRAM(i);

  else if(destination < wramx_end) //
    std::ranges::copy_n(m_builtIn.m_wram.begin(), amount, m_ppu.m_oam.begin());

  else {
    // ignore write
  }
  m_clock.cycle(160);
}

}
