#include <LR35902/config.h>
#include <LR35902/stubs/cartridge/kind/rom_only.h>

namespace LR35902 {

byte rom_only::read(const std::size_t index) const noexcept {
  return m_rom[index];
}

void rom_only::write(const std::size_t index, const byte b) noexcept {
  // writes has no effect when rom only (rom-> *read only* memory)
  // on rom_ram it may be used for ram bank selection
  (void)index;
  (void)b;
}

auto rom_only::data() const noexcept -> decltype(m_rom) {
  return m_rom;
}

}
