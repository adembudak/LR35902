#include <LR35902/config.h>
#include <LR35902/memory_map.h>
#include <LR35902/stubs/builtin/builtin.h>
#include <LR35902/stubs/bus/bus.h>
#include <LR35902/stubs/cartridge/cartridge.h>
#include <LR35902/stubs/io/io.h>
#include <LR35902/stubs/ppu/ppu.h>

namespace LR35902 {

Bus::Bus(Cartridge &cart, PPU &ppu, BuiltIn &builtIn, IO &io) :
    m_cart{cart},
    m_ppu{ppu},
    m_builtIn{builtIn},
    m_io{io} {}

byte &Bus::read_write(const std::size_t index) noexcept {
  if(index < romx_end) return m_cart[index];
  else if(index < vram_end) return m_ppu[index];
  else if(index < sram_end) return m_cart[index];
  else if(index < wramx_end) return m_builtIn[index];
  else if(index < echo_end) return m_builtIn[index];
  else if(index < oam) return m_ppu[index];
  else if(index < noUsable_end) return m_builtIn[index];
  else if(index < io_end) return m_io[index];
  else if(index < hram_end) return m_builtIn[index];
  // return {}; // REVISIT: implement this, IE interrupt stub
}

byte Bus::read(const std::size_t index) const noexcept {
  if(index < romx_end) return m_cart.readROM(index);
  else if(index < vram_end) return m_ppu.readVRAM(index - vram);
  else if(index < sram_end) return m_cart.readSRAM(index - sram);
  else if(index < wramx_end) return m_builtIn.readWRAM(index - wram0);
  else if(index < echo_end) return m_builtIn.readEcho(index - echo);
  else if(index < oam) return m_ppu.readOAM(index - oam);
  else if(index < noUsable_end) return m_builtIn.readNoUsable(index - noUsable);
  else if(index < io_end) return m_io.readIO(index - io);
  else if(index < hram_end) return m_builtIn.readHRAM(index - hram);
  return {}; // REVISIT: implement this, IE interrupt stub
}

void Bus::write(const std::size_t index, const byte b) noexcept {
  if(index < romx_end) return m_cart.writeROM(index, b);
  else if(index < vram_end) m_ppu.writeVRAM(index - vram, b);
  else if(index < sram_end) m_cart.writeSRAM(index - sram, b);
  else if(index < wramx_end) m_builtIn.writeWRAM(index - wram0, b);
  else if(index < echo_end) m_builtIn.writeEcho(index - echo, b);
  else if(index < oam) m_ppu.writeOAM(index - oam, b);
  else if(index < noUsable_end) m_builtIn.writeNoUsable(index - noUsable, b);
  else if(index < io_end) m_io.writeIO(index, b);
  else if(index < hram_end) m_builtIn.writeHRAM(index - hram, b);
  //  return {}; // REVISIT
}

}
