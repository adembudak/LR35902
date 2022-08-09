#include <LR35902/builtin/builtin.h>
#include <LR35902/cartridge/cartridge.h>
#include <LR35902/dma/dma.h>
#include <LR35902/memory_map.h>
#include <LR35902/ppu/ppu.h>

#include <cassert>
#include <cstddef>

namespace LR35902 {

DMA::DMA(Cartridge &cart, PPU &ppu, BuiltIn &builtIn) :
    m_cart{cart},
    m_ppu{ppu},
    m_builtIn{builtIn} {}

void DMA::action(const byte n) noexcept {
  const std::size_t amount = 0xa0; // 40 * 32 bit == 160 byte
  const std::size_t destination = n * 0x100;

  if(destination < romx_end) {
    for(std::size_t i = 0; i < amount; ++i)
      m_ppu.writeOAM(i, m_cart.readROM(i));
  }

  else if(destination < vram_end) {
    for(std::size_t i = 0; i < amount; ++i)
      m_ppu.writeOAM(i, m_ppu.readVRAM(i));
  }

  else if(destination < sram_end) {
    for(std::size_t i = 0; i < amount; ++i)
      m_ppu.writeOAM(i, m_cart.readSRAM(i));
  }

  else if(destination < wramx_end) {
    for(std::size_t i = 0; i < amount; ++i)
      m_ppu.writeOAM(i, m_builtIn.readWRAM(i));
  }

  else {
    assert(false);
  }
  // clock.cycle(160);
}

}
