#include <LR35902/stubs/bus/bus.h>
#include <LR35902/stubs/cartridge/cartridge.h>

namespace LR35902 {

Bus::Bus(Cartridge &cart, PPU &ppu, BuiltIn &builtIn) :
    m_cart{cart},
    m_ppu{ppu},
    m_builtIn{builtIn} {}

byte &Bus::read_write(const std::size_t index) noexcept {
  return m_cart[index];
}

byte Bus::read(const std::size_t index) const noexcept {
  return m_cart.read(index);
}

void Bus::write(const std::size_t index, const byte b) noexcept {
  m_cart.write(index, b);
}

}
