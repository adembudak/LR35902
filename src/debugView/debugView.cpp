#include <LR35902/core.h>
#include <LR35902/debugView/debugView.h>
#include <LR35902/stubs/cartridge/cartridge.h>

#include <imgui.h>
#include <imgui_memory_editor/imgui_memory_editor.h>

namespace LR35902 {

void DebugView::registerStatus(const Core &m_core) noexcept {
  using namespace ImGui;

  Begin("Registers:");
  // clang-format off
    Text("A: %u", m_core.A.data()); 
    Text("F: %u", m_core.F.data()); 

    ImGui::NewLine();
    Text("B: %u", m_core.B.data()); SameLine(); Text("C: %u", m_core.C.data()); Text("BC: %u", m_core.BC.data());

    ImGui::NewLine();
    Text("D: %u", m_core.D.data()); SameLine(); Text("E: %u", m_core.E.data()); Text("DE: %u", m_core.DE.data());

    ImGui::NewLine();
    Text("H: %u", m_core.H.data()); SameLine(); Text("L: %u", m_core.L.data()); Text("HL: %u", m_core.HL.data());

    ImGui::NewLine();
    Text("SP: %u", m_core.SP.m_data); Text("PC: %u", m_core.PC.m_data);

    ImGui::NewLine();
    ImGui::NewLine();
    Text("%.3f ms/frame (%.1f FPS)", 1/ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
  // clang-format on
  End();
}

void DebugView::romDump(const Cartridge &cart) noexcept {
  static MemoryEditor mem_editor;
  mem_editor.ReadOnly = true;

  mem_editor.DrawWindow("memory", (void *)cart.m_rom.data(), cart.m_rom.size());
}

}
