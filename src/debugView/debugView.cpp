#include <LR35902/core/core.h>
#include <LR35902/debugView/debugView.h>
#include <LR35902/memory_map.h>
#include <LR35902/stubs/builtin/builtin.h>
#include <LR35902/stubs/cartridge/cartridge.h>
#include <LR35902/stubs/io/io.h>
#include <LR35902/stubs/ppu/ppu.h>

#include <imgui/imgui.h>
#include <imgui_memory_editor/imgui_memory_editor.h>

#include <cstdint>

namespace LR35902 {

void DebugView::showMemoryPortions() noexcept {
  using namespace ImGui;

  if(_memory_portions) {
    Begin("Memory Portions", &_memory_portions);

    if(static MemoryEditor memory_editor; BeginTabBar("Tab Bar")) {
      memory_editor.ReadOnly = true;

      if(BeginTabItem("rom")) {
        memory_editor.DrawContents((void *)m_cart.data(), 32_KiB, rom0);

        EndTabItem();
      }

      if(BeginTabItem("vram")) {
        memory_editor.DrawContents((void *)std::data(m_ppu.m_vram), std::size(m_ppu.m_vram), vram);

        EndTabItem();
      }

      if(m_cart.hasSRAM()) {
        if(BeginTabItem("sram")) {
          memory_editor.DrawContents((void *)m_cart.data(), 8_KiB, sram);

          EndTabItem();
        }
      }

      if(BeginTabItem("wram")) {
        memory_editor.DrawContents((void *)std::data(m_builtIn.m_wram), std::size(m_builtIn.m_wram), wram0);

        EndTabItem();
      }

      if(BeginTabItem("echo")) {
        memory_editor.DrawContents((void *)std::data(m_builtIn.m_echo), std::size(m_builtIn.m_echo), echo);

        EndTabItem();
      }

      if(BeginTabItem("oam")) {
        memory_editor.DrawContents((void *)std::data(m_ppu.m_oam), std::size(m_ppu.m_oam), oam);

        EndTabItem();
      }

      if(BeginTabItem("noUsable")) {
        memory_editor.DrawContents((void *)std::data(m_builtIn.m_noUsable), std::size(m_builtIn.m_noUsable),
                                   noUsable);

        EndTabItem();
      }

      if(BeginTabItem("io")) {
        memory_editor.DrawContents((void *)std::data(m_io.m_data), std::size(m_io.m_data), io);

        EndTabItem();
      }

      if(BeginTabItem("hram")) {
        memory_editor.DrawContents((void *)std::data(m_builtIn.m_hram), std::size(m_builtIn.m_hram), hram);

        EndTabItem();
      }

      EndTabBar();
    }

    End();
  }
}

void DebugView::showDisassembly() noexcept {
  using namespace ImGui;

  if(_disassembly) {
    Begin("Disassembly", &_disassembly);

    // TODO: implement this
    Text("%s\n", "Disassembly");

    End();
  }
}

void DebugView::showCPUState() noexcept {
  using namespace ImGui;

  if(_cpu_state) {
    Begin("CPU State", &_cpu_state, ImGuiWindowFlags_NoResize);

    Text("A: %u", m_core.A.data());
    const char Z = m_core.F.data() & 0b1000'0000 ? 'Z' : '-';
    const char N = m_core.F.data() & 0b0100'0000 ? 'N' : '-';
    const char H = m_core.F.data() & 0b0010'0000 ? 'H' : '-';
    const char C = m_core.F.data() & 0b0001'0000 ? 'C' : '-';
    Text("Flags: %c %c %c %c", Z, N, H, C);

    NewLine();
    Text("B C: %x %x", m_core.B.data(), m_core.C.data());

    NewLine();
    Text("D E: %x %x", m_core.D.data(), m_core.E.data());

    NewLine();
    Text("H L: %x %x", m_core.H.data(), m_core.L.data());

    NewLine();
    Text("SP: %u", m_core.SP.m_data);
    Text("PC: %u", m_core.PC.m_data);

    NewLine();
    NewLine();
    Text("%.3f ms/frame (%.1f FPS)", 1 / GetIO().Framerate, GetIO().Framerate);

    End();
  }
}

bool showRegisters_ = true;
bool showJoypadRegister = false;
bool showSerialCableRegisters = false;
bool showTimerRegisters = false;
bool showAudioRegisters = false;
bool showLCDRegistesr = false;

void DebugView::showRegisters() noexcept {
  using namespace ImGui;

  if(_registers) {
    Begin("Registers", &_registers);

    if(Checkbox("LCD", &showLCDRegistesr); showLCDRegistesr) {
      Text("LCDC: %x", m_io.LCDC);
      Text("STAT: %x", m_io.STAT);

      Text("SCY: %x", m_io.SCY);
      Text("SCX: %x", m_io.SCX);

      Text("LY:  %x", m_io.LY);
      Text("LYC: %x", m_io.LYC);

      Text("DMA: %x", m_io.DMA);

      Text("BGP: %x", m_io.BGP);
      Text("OBP0: %x", m_io.OBP0);
      Text("OBP1: %x", m_io.OBP1);

      Text("WY: %x", m_io.WY);
      Text("WX: %x", m_io.WX);

      Separator();
    }

    if(Checkbox("Timer", &showTimerRegisters); showTimerRegisters) {
      Text("DIV: %x\n", m_io.DIV);
      Text("TIMA: %x\n", m_io.TIMA);
      Text("TMA: %x\n", m_io.TMA);
      Text("TAC: %x\n", m_io.TAC);

      Separator();
    }

    if(Checkbox("Joypad", &showJoypadRegister); showJoypadRegister) {
      Text("P1: %x", m_io.P1);

      Separator();
    }

    if(Checkbox("Serial Cable", &showSerialCableRegisters); showSerialCableRegisters) {
      Text("SB: %x\n", m_io.SB);
      Text("SC: %x\n", m_io.SC);

      Separator();
    }

    if(Checkbox("Audio", &showAudioRegisters); showAudioRegisters) {
      Text("NR10: %x\n", m_io.NR10);
      Text("NR11: %x\n", m_io.NR11);
      Text("NR12: %x\n", m_io.NR12);
      Text("NR13: %x\n", m_io.NR13);
      Text("NR14: %x\n", m_io.NR14);

      Text("NR21: %x\n", m_io.NR21);
      Text("NR22: %x\n", m_io.NR22);
      Text("NR23: %x\n", m_io.NR23);
      Text("NR24: %x\n", m_io.NR24);

      Text("NR30: %x\n", m_io.NR30);
      Text("NR31: %x\n", m_io.NR31);
      Text("NR32: %x\n", m_io.NR32);
      Text("NR33: %x\n", m_io.NR33);
      Text("NR34: %x\n", m_io.NR34);

      Text("NR41: %x\n", m_io.NR41);
      Text("NR42: %x\n", m_io.NR42);
      Text("NR43: %x\n", m_io.NR43);
      Text("NR44: %x\n", m_io.NR44);
      Text("NR50: %x\n", m_io.NR50);
      Text("NR51: %x\n", m_io.NR51);
      Text("NR52: %x\n", m_io.NR52);
    }

    /*
    // interrupt registers
    byte &IF = m_data[0x0f];
    */

    End();
  }
}

void DebugView::showCartridgeHeader() noexcept {
  using namespace ImGui;

  if(_cartridge_header) {
    Begin("Cartridge Header", &_cartridge_header);

    const auto header = m_cart.CartridgeHeader;

    Text("Logo check: %s\n", header.nintendo_logo_check ? "Pass" : "Fail");
    Text("Title:      %s\n", header.title);
    Text("MBC:        %d\n", (int)header.kind);
    Text("Checksum:   %s\n", header.checksum ? "Pass" : "Fail");
    Text("ROM size:   %s\n", header.rom_size.data());
    Text("RAM size:   %s\n", header.ram_size.data());

    End();
  }
}

}
