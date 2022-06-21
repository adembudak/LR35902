#include <LR35902/core.h>
#include <LR35902/debugView/debugView.h>
#include <LR35902/stubs/cartridge/cartridge.h>

#include <imgui/imgui.h>
#include <imgui_memory_editor/imgui_memory_editor.h>

namespace LR35902 {

void DebugView::registerStatus(const Core &core) noexcept {
  using namespace ImGui;

  Begin("Registers:", NULL, ImGuiWindowFlags_NoResize);

  Text("A: %u", core.A.data());
  const char Z = core.F.data() & 0b1000'0000 ? 'Z' : '-';
  const char N = core.F.data() & 0b0100'0000 ? 'N' : '-';
  const char H = core.F.data() & 0b0010'0000 ? 'H' : '-';
  const char C = core.F.data() & 0b0001'0000 ? 'C' : '-';
  Text("Flags: %c %c %c %c", Z, N, H, C);

  NewLine();
  Text("B C: %x %x", core.B.data(), core.C.data());

  NewLine();
  Text("D E: %x %x", core.D.data(), core.E.data());

  NewLine();
  Text("H L: %x %x", core.H.data(), core.L.data());

  NewLine();
  Text("SP: %u", core.SP.m_data);
  Text("PC: %u", core.PC.m_data);

  NewLine();
  NewLine();
  Text("%.3f ms/frame (%.1f FPS)", 1 / GetIO().Framerate, GetIO().Framerate);

  End();
}

void DebugView::dumpROM(const Cartridge &cart) noexcept {
  static MemoryEditor memory_editor_rom;
  memory_editor_rom.ReadOnly = true;
  memory_editor_rom.DrawWindow("ROM", (void *)cart.m_rom.data(), 16_KiB);
}

}
