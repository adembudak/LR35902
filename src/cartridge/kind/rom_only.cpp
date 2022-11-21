#include <LR35902/cartridge/kind/rom_only.h>
#include <LR35902/config.h>
#include <LR35902/memory_map.h>

#include <cstdint>

namespace LR35902 {

byte rom_only::read(const std::size_t index) const noexcept {
  return m_rom[index];
}

void rom_only::write(const std::size_t index, const byte b) noexcept {
  // writes has no effect when the cart is rom_only (without static ram, also rom: *read only* memory)
  // on rom_ram it may be used for ram bank selection, when ram size > 8_KiB
  (void)index;
  (void)b;
}

}
