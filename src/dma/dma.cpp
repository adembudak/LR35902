#include <LR35902/builtin/builtin.h>
#include <LR35902/cartridge/cartridge.h>
#include <LR35902/config.h>
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

constexpr std::size_t numberOfBytesToTransfer = 160_B; // 40 * 32 bits == 40 * 4 bytes == 160 bytes

void DMA::action(const byte n) noexcept {
  address_t offset = n * 0x100;

  if(offset < mmap::romx_end) {
    for(std::size_t i = 0; i < numberOfBytesToTransfer; ++i)
      m_ppu.m_oam[i] = m_cart.readROM(offset + i);
  }

  else if(offset < mmap::vram_end) {
    offset = normalize_index(offset, mmap::vram);
    std::ranges::copy_n(m_ppu.m_vram.begin() + offset, numberOfBytesToTransfer, m_ppu.m_oam.begin());
  }

  else if(offset < mmap::sram_end) {
    offset = normalize_index(offset, mmap::sram);
    for(std::size_t i = 0; i < numberOfBytesToTransfer; ++i)
      m_ppu.m_oam[i] = m_cart.readSRAM(offset + i);
  }

  else if(offset < mmap::wramx_end) {
    offset = normalize_index(offset, mmap::wram0);
    std::ranges::copy_n(m_builtIn.m_wram.begin() + offset, numberOfBytesToTransfer, m_ppu.m_oam.begin());
  }

  else {
    // ignore write
  }
  m_clock.cycle(160);
}

}
