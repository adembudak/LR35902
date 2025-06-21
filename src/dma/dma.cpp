#include <LR35902/builtin/builtin.h>
#include <LR35902/cartridge/cartridge.h>
#include <LR35902/config.h>
#include <LR35902/cpu/clock/clock.h>
#include <LR35902/dma/dma.h>
#include <LR35902/memory_map.h>
#include <LR35902/ppu/ppu.h>

#include <mpark/patterns/match.hpp>

#include <cassert>
#include <cstddef>
#include <ranges>

namespace LR35902 {

DMA::DMA(Cartridge &cart, PPU &ppu, BuiltIn &builtIn, Clock &clock) :
    m_cart{cart},
    m_ppu{ppu},
    m_builtIn{builtIn},
    m_clock{clock} {}

constexpr std::size_t numberOfBytesToTransfer = 160_B; // 40 * 32 bits == 40 * 4 bytes == 160 bytes

void DMA::action(const byte n) noexcept {
  address_t offset = n * 0x100;

  using namespace mpark::patterns;
  match(offset)(
    pattern(_).when(_ >= mmap::rom0 && _ < mmap::romx_end) = [&] {
            for(const address_t i : std::views::iota(0u, numberOfBytesToTransfer))
              m_ppu.writeOAM(i, m_cart.readROM(offset + i));
      },
    pattern(_).when(_ >= mmap::vram && _ < mmap::vram_end) = [&] {
            offset = normalize_index(offset, mmap::vram);

            for(const address_t i : std::views::iota(0u, numberOfBytesToTransfer))
	       m_ppu.writeOAM(i, m_ppu.readVRAM(offset + i));

      },
    pattern(_).when(_ >= mmap::sram && _ < mmap::sram_end) = [&] {
            offset = normalize_index(offset, mmap::sram);

            for(const address_t i : std::views::iota(0u, numberOfBytesToTransfer))
	       m_ppu.writeOAM(i, m_cart.readSRAM(offset + i));
      },
    pattern(_).when(_ >= mmap::wram0 && _ < mmap::wramx_end) = [&] {
            offset = normalize_index(offset, mmap::wram0);

            for(const address_t i : std::views::iota(0u, numberOfBytesToTransfer))
              m_ppu.writeOAM(i, m_builtIn.readWRAM(offset + i));
      },
    pattern(_) = [] {

      }
  );

  m_clock.cycle(160);
}

}
