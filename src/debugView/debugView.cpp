#include <LR35902/core/core.h>
#include <LR35902/debugView/debugView.h>
#include <LR35902/stubs/builtin/builtin.h>
#include <LR35902/stubs/cartridge/cartridge.h>
#include <LR35902/stubs/io/io.h>
#include <LR35902/stubs/ppu/ppu.h>

#include <imgui/imgui.h>
#include <imgui_memory_editor/imgui_memory_editor.h>

namespace LR35902 {

void DebugView::CPURegisters(const Core &core) noexcept {
  using namespace ImGui;

  Begin("CPU: ", NULL, ImGuiWindowFlags_NoResize);

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

bool showJoypadRegister = false;
bool showSerialCableRegisters = false;
bool showTimerRegisters = false;
bool showAudioRegisters = false;
bool showLCDRegistesr = false;

void DebugView::registers(const IO &io) {
  using namespace ImGui;

  Begin("Registers:", NULL);

  if(Checkbox("LCD", &showLCDRegistesr); showLCDRegistesr) {
    Text("LCDC: %x", io.LCDC);
    Text("STAT: %x", io.STAT);

    Text("SCY: %x", io.SCY);
    Text("SCX: %x", io.SCX);

    Text("LY:  %x", io.LY);
    Text("LYC: %x", io.LYC);

    Text("DMA: %x", io.DMA);

    Text("BGP: %x", io.BGP);
    Text("OBP0: %x", io.OBP0);
    Text("OBP1: %x", io.OBP1);

    Text("WY: %x", io.WY);
    Text("WX: %x", io.WX);

    Separator();
  }

  if(Checkbox("Timer", &showTimerRegisters); showTimerRegisters) {
    Text("DIV: %x\n", io.DIV);
    Text("TIMA: %x\n", io.TIMA);
    Text("TMA: %x\n", io.TMA);
    Text("TAC: %x\n", io.TAC);

    Separator();
  }

  if(Checkbox("Joypad", &showJoypadRegister); showJoypadRegister) {
    Text("P1: %x", io.P1);

    Separator();
  }

  if(Checkbox("Serial Cable", &showSerialCableRegisters); showSerialCableRegisters) {
    Text("SB: %x\n", io.SB);
    Text("SC: %x\n", io.SC);

    Separator();
  }

  if(Checkbox("Audio", &showAudioRegisters); showAudioRegisters) {
    Text("NR10: %x\n", io.NR10);
    Text("NR11: %x\n", io.NR11);
    Text("NR12: %x\n", io.NR12);
    Text("NR13: %x\n", io.NR13);
    Text("NR14: %x\n", io.NR14);

    Text("NR21: %x\n", io.NR21);
    Text("NR22: %x\n", io.NR22);
    Text("NR23: %x\n", io.NR23);
    Text("NR24: %x\n", io.NR24);

    Text("NR30: %x\n", io.NR30);
    Text("NR31: %x\n", io.NR31);
    Text("NR32: %x\n", io.NR32);
    Text("NR33: %x\n", io.NR33);
    Text("NR34: %x\n", io.NR34);

    Text("NR41: %x\n", io.NR41);
    Text("NR42: %x\n", io.NR42);
    Text("NR43: %x\n", io.NR43);
    Text("NR44: %x\n", io.NR44);
    Text("NR50: %x\n", io.NR50);
    Text("NR51: %x\n", io.NR51);
    Text("NR52: %x\n", io.NR52);
  }

  /*
  // interrupt registers
  byte &IF = m_data[0x0f];
  */

  End();
}

void DebugView::dumpROM(const Cartridge &cart) noexcept {
  static MemoryEditor memory_editor_rom;
  memory_editor_rom.ReadOnly = true;

  memory_editor_rom.DrawWindow("ROM", (void *)std::data(cart.m_rom), std::size(cart.m_rom));
}

void DebugView::dumpSRAM(const Cartridge &cart) noexcept {
  static MemoryEditor memory_editor_sram;
  memory_editor_sram.ReadOnly = true;

  memory_editor_sram.DrawWindow("SRAM", (void *)std::data(cart.m_sram), std::size(cart.m_sram));
}

void DebugView::dumpVRAM(const PPU &ppu) noexcept {
  static MemoryEditor memory_editor_vram;
  memory_editor_vram.ReadOnly = true;

  memory_editor_vram.DrawWindow("VRAM", (void *)std::data(ppu.m_vram), std::size(ppu.m_vram));
}

void DebugView::dumpOAM(const PPU &ppu) noexcept {
  static MemoryEditor memory_editor_oam;
  memory_editor_oam.ReadOnly = true;

  memory_editor_oam.DrawWindow("OAM", (void *)std::data(ppu.m_oam), std::size(ppu.m_oam));
}

void DebugView::dumpWRAM(const BuiltIn &builtin) noexcept {
  static MemoryEditor memory_editor_wram;
  memory_editor_wram.ReadOnly = true;

  memory_editor_wram.DrawWindow("WRAM", (void *)std::data(builtin.m_wram), std::size(builtin.m_wram));
}

void DebugView::dumpHRAM(const BuiltIn &builtin) noexcept {
  static MemoryEditor memory_editor_hram;
  memory_editor_hram.ReadOnly = true;

  memory_editor_hram.DrawWindow("WRAM", (void *)std::data(builtin.m_hram), std::size(builtin.m_hram));
}

}
