#include "../../debugger/GameBoy.h"

#include <LR35902/debugView/debugView.h>
#include <LR35902/memory_map.h>

#include <imgui/imgui.h>
#include <imgui_memory_editor/imgui_memory_editor.h>

#include <cstdint>

namespace LR35902 {

DebugView::DebugView(const GameBoy &gameboy) :
    gameboy{gameboy} {
  _memory_portions_sram = gameboy.cart.hasSRAM();
}

void DebugView::showMemoryPortions() noexcept {
  using namespace ImGui;

  if(_memory_portions) {
    Begin("Memory Portions", &_memory_portions);

    if(static MemoryEditor memory_editor; BeginTabBar("Tab Bar")) {
      memory_editor.ReadOnly = true;

      if(BeginTabItem("rom", &_memory_portions_rom)) {
        memory_editor.DrawContents((void *)gameboy.cart.data(), 32_KiB, rom0);

        EndTabItem();
      }

      if(BeginTabItem("vram", &_memory_portions_vram)) {
        memory_editor.DrawContents((void *)std::data(gameboy.ppu.m_vram), std::size(gameboy.ppu.m_vram),
                                   vram);

        EndTabItem();
      }

      if(gameboy.cart.hasSRAM()) {
        if(BeginTabItem("sram", &_memory_portions_sram)) {
          memory_editor.DrawContents((void *)gameboy.cart.data(), 8_KiB, sram);

          EndTabItem();
        }
      }

      if(BeginTabItem("wram", &_memory_portions_wram)) {
        memory_editor.DrawContents((void *)std::data(gameboy.builtIn.m_wram),
                                   std::size(gameboy.builtIn.m_wram), wram0);

        EndTabItem();
      }

      if(BeginTabItem("echo", &_memory_portions_echo)) {
        memory_editor.DrawContents((void *)std::data(gameboy.builtIn.m_echo),
                                   std::size(gameboy.builtIn.m_echo), echo);

        EndTabItem();
      }

      if(BeginTabItem("oam", &_memory_portions_oam)) {
        memory_editor.DrawContents((void *)std::data(gameboy.ppu.m_oam), std::size(gameboy.ppu.m_oam), oam);

        EndTabItem();
      }

      if(BeginTabItem("noUsable", &_memory_portions_noUsable)) {
        memory_editor.DrawContents((void *)std::data(gameboy.builtIn.m_noUsable),
                                   std::size(gameboy.builtIn.m_noUsable), noUsable);

        EndTabItem();
      }

      if(BeginTabItem("io", &_memory_portions_io)) {
        memory_editor.DrawContents((void *)std::data(gameboy.io.m_data), std::size(gameboy.io.m_data), io);

        EndTabItem();
      }

      if(BeginTabItem("hram", &_memory_portions_hram)) {
        memory_editor.DrawContents((void *)std::data(gameboy.builtIn.m_hram),
                                   std::size(gameboy.builtIn.m_hram), hram);

        EndTabItem();
      }

      EndTabBar();
    }

    End();
  }
}

// Example
// PC     opcode                         instruction
// ---------------------------------------------------
// 1      mem[pc] mem[pc+1]              ld a, $12
// 3      mem[pc]                        ld a, b
// 4      mem[pc] mem[pc+1] mem[pc+2]    ld hl, $1234
// 6      mem[pc]                        sfc

void DebugView::showDisassembly() noexcept {
  using namespace ImGui;

  if(_disassembly) {

    Begin("Disassembly", &_disassembly, ImGuiWindowFlags_NoCollapse);
    const auto core = gameboy.core;

    using enum Core::OpcodeKind;
    switch(core.kind) {
    // clang-format off
      case opcode:         instructions.insert_or_assign(core.PC.m_data, std::tuple{core.kind, core.opcode, std::nullopt}); break;
      case opcode_reg_n8:  instructions.insert_or_assign(core.PC.m_data, std::tuple{core.kind, core.opcode, core.immediate_byte}); break;
      case opcode_reg_n16: instructions.insert_or_assign(core.PC.m_data, std::tuple{core.kind, core.opcode, core.immediate_word}); break;
      case opcode_e8:      instructions.insert_or_assign(core.PC.m_data, std::tuple{core.kind, core.opcode, std::int8_t(core.immediate_byte)}); break;
    }

    for(const auto &[pc, operation] : instructions) {
      const auto &[Kind, Opcode, Immediate] = operation;
      const auto PC = pc - 1;
      switch(Kind) {
        case opcode:         ImGui::Text("%04x  %02x\n", PC, Opcode); break;
        case opcode_reg_n8:  ImGui::Text("%04x  %02x %02x\n", PC, Opcode, *Immediate); break;
        case opcode_reg_n16: ImGui::Text("%04x  %02x %02x %02x\n", PC, Opcode, *Immediate & 0x00ff, (*Immediate & 0xff00) >> 8); break;
        case opcode_e8:      ImGui::Text("%04x  %02x %02x", PC, Opcode, *Immediate); break;
        // clang-format on
      }
    }

    End();
  }
}

void DebugView::showCPUState() noexcept {
  using namespace ImGui;

  if(_cpu_state) {
    Begin("CPU State", &_cpu_state, ImGuiWindowFlags_NoResize);
    const auto &core = gameboy.core;

    Text("Cycle: %lu", core.m_clock.m_data);

    NewLine();
    Text("ime: %d", core.ime);

    NewLine();
    Text("A: %02x", core.A.data());
    const bool Z = core.F.data() & 0b1000'0000;
    const bool N = core.F.data() & 0b0100'0000;
    const bool H = core.F.data() & 0b0010'0000;
    const bool C = core.F.data() & 0b0001'0000;
    Text("Flags: Z N H C\n"
         "       %d %d %d %d",
         Z, N, H, C);

    NewLine();
    Text("B C: %x %02x", core.B.data(), core.C.data());

    NewLine();
    Text("D E: %x %02x", core.D.data(), core.E.data());

    NewLine();
    Text("H L: %x %02x", core.H.data(), core.L.data());

    NewLine();
    Text("SP: %u", core.SP.m_data);
    Text("PC: %u", core.PC.m_data);

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
    const auto &io = gameboy.io;

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

    // interrupt registers
    // byte &IF = m_data[0x0f];

    End();
  }
}

void DebugView::showCartridgeHeader() noexcept {
  using namespace ImGui;

  if(_cartridge_header) {
    Begin("Cartridge Header", &_cartridge_header);

    const auto &header = gameboy.cart.CartridgeHeader;

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
