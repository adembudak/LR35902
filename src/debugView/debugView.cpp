#include "../../debugger/GameBoy.h"

#include <LR35902/debugView/debugView.h>
#include <LR35902/memory_map.h>

#include <imgui/imgui.h>
#include <imgui_memory_editor/imgui_memory_editor.h>

#include <algorithm>
#include <cstdint>
#include <variant>

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

void DebugView::showDisassembly() noexcept {
  using namespace ImGui;

  if(_disassembly) {
    Begin("Disassembly", &_disassembly);

    iv.insert_or_assign(gameboy.cpu.PC.m_data, std::pair{gameboy.cpu.opcode, gameboy.cpu.immediate});

    for(const auto &[PC, op] : iv) {
      const auto [opcode, immediate] = op;
      if(std::holds_alternative<byte>(immediate))
        Text("%04x  %02x %02x", PC, opcode, std::get<byte>(immediate));
      else if(std::holds_alternative<word>(immediate))
        Text("%04x  %02x %02x", PC, opcode, std::get<word>(immediate));
      else Text("%04x  %02x", PC, opcode);
    }

    End();
  }
}

void DebugView::showCPUState() noexcept {
  using namespace ImGui;

  if(_cpu_state) {
    Begin("CPU State", &_cpu_state, ImGuiWindowFlags_NoResize);
    const auto &cpu = gameboy.cpu;

    Text("Cycle: %lu", cpu.m_clock.m_data);

    NewLine();
    Text("ime: %d", cpu.ime);

    NewLine();
    Text("A: %02x", cpu.A.data());
    const bool Z = cpu.F.data() & 0b1000'0000;
    const bool N = cpu.F.data() & 0b0100'0000;
    const bool H = cpu.F.data() & 0b0010'0000;
    const bool C = cpu.F.data() & 0b0001'0000;
    Text("Flags: Z N H C\n"
         "       %d %d %d %d",
         Z, N, H, C);

    NewLine();
    Text("B C: %x %02x", cpu.B.data(), cpu.C.data());

    NewLine();
    Text("D E: %x %02x", cpu.D.data(), cpu.E.data());

    NewLine();
    Text("H L: %x %02x", cpu.H.data(), cpu.L.data());

    NewLine();
    Text("SP: %u", cpu.SP.m_data);
    Text("PC: %u", cpu.PC.m_data);

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
bool showLCDRegisters = false;
bool showInterruptRegisters = false;

void DebugView::showRegisters() noexcept {
  using namespace ImGui;

  if(_registers) {
    Begin("Registers", &_registers);
    const auto &io = gameboy.io;

    if(Checkbox("LCD", &showLCDRegisters); showLCDRegisters) {
      Text("LCDC: %x", io.LCDC);
      Text("STAT: %x", io.STAT);

      switch(io.STAT & 0b11) {
      case 0b00: Text("status : Horizontal Blank"); break;
      case 0b01: Text("status : Vertical Blank"); break;
      case 0b10: Text("status : Searching OAM"); break;
      case 0b11: Text("status : Draw"); break;
      }

      Text("SCY: %x", io.SCY);
      Text("SCX: %x", io.SCX);

      Text("LY:  %u", io.LY);
      Text("LYC: %u", io.LYC);

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

    if(Checkbox("Interrupts", &showInterruptRegisters); showInterruptRegisters) {
      const auto &IE = gameboy.intr.IE;
      const auto &IF = gameboy.intr.IF;

      Text("IME: %d ", gameboy.cpu.ime); // interrupt master enable
      Text("IE: %x ", IE);
      Text("IE: %x ", IF);

      static ImGuiTableFlags flags = ImGuiTableFlags_SizingFixedFit  //
                                     | ImGuiTableFlags_RowBg         //
                                     | ImGuiTableFlags_Borders       //
                                     | ImGuiTableFlags_NoHostExtendX //
                                     | ImGuiTableFlags_Hideable;

      const char *const kind[6]{"", "vblank", "lcd_stat", "timer", "serial", "joypad"};
      if(ImGui::BeginTable("Interrupts", /*columns*/ 6, flags)) {
        for(int i = 0; i < 6; ++i)
          ImGui::TableSetupColumn(kind[i]);
        ImGui::TableHeadersRow();

        ImGui::TableNextRow();
        ImGui::TableSetColumnIndex(0);
        ImGui::Text("%s", "(IE) Enabled: ");
        for(int column = 1; column < 6; ++column) {
          ImGui::TableSetColumnIndex(column);
          ImGui::Text("%d", bool(IE & (0b0000'0001 << (column - 1))));
        }

        ImGui::TableNextRow();
        ImGui::TableSetColumnIndex(0);
        ImGui::Text("%s", "(IF) Requested: ");
        for(int column = 1; column < 6; ++column) {
          ImGui::TableSetColumnIndex(column);
          ImGui::Text("%d", bool(IF & (0b0000'0001 << (column - 1))));
        }

        ImGui::EndTable();
      }
    }

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

using palette_t = std::array<ImColor, 4>;

// https://www.deviantart.com/thewolfbunny64/art/Game-Boy-Palette-Gamate-Ver-808006887
const palette_t original = {
    ImColor{107, 166, 74}, //
    ImColor{67,  122, 99}, //
    ImColor{37,  89,  85}, //
    ImColor{18,  66,  76}
};

// https://www.deviantart.com/thewolfbunny64/art/Game-Boy-Palette-Cola-Cola-Red-808488517
const palette_t cococola = {
    ImColor{244, 0, 9}, //
    ImColor{186, 0, 6}, //
    ImColor{114, 0, 4}, //
    ImColor{43,  0, 1}
};

const palette_t galata = {
    ImColor{117, 148, 166},
    ImColor{101, 130, 144},
    ImColor{32,  46,  72 },
    ImColor{34,  63,  69 }
};

void DebugView::visualizeVRAM() noexcept {
  using namespace ImGui;

  if(_vram) {
    Begin("VRAM", &_vram);

    palette_t palette = original;

    static int selected = 0;

    SetNextItemWidth(100.0f);
    const char *items[]{"Original", "Coco'Cola", "Galata"};
    Combo("Palette", &selected, items, IM_ARRAYSIZE(items));
    switch(selected) {
    case 0: palette = original; break;
    case 1: palette = cococola; break;
    case 2: palette = galata; break;
    }

    ImDrawList *const draw_list = ImGui::GetWindowDrawList();
    auto put_pixel = [&draw_list](const float x, const float y, const ImColor color) {
      const ImVec2 p = ImGui::GetCursorScreenPos();

      draw_list->AddRectFilled({p.x + x, p.y + y}, {p.x + x + 1, p.y + y + 1}, color);
    };

    using tile = std::array<byte, PPU::tile_size>;
    tile tile_x{};
    ///////////////////////

    // iterate tile
    // 0 -> 6144, += 16  (6144 == 6_KiB == tilemap size)
    int x = 0;
    for(std::size_t tile_nth = 0; tile_nth < PPU::tilemap_size; tile_nth += PPU::tile_size, x += 8) {
      std::copy_n(gameboy.ppu.m_vram.begin() + tile_nth, PPU::tile_size, tile_x.begin());
      if(std::all_of(begin(tile_x), end(tile_x), [](byte b) { return b == 0; })) continue;

      int y = 0;
      // iterate tilelines
      // 0 -> 16, += 2
      for(std::size_t tileline_nth = 0; tileline_nth < PPU::tile_size;
          tileline_nth += PPU::tileline_size, ++y) {

        byte mask = 0b1000'0000;
        // iterate a single tile tileline
        for(std::size_t pixel_nth = 0; pixel_nth < PPU::tile_w; ++pixel_nth, mask >>= 1) { // iterates 0->7
          const bool index_1 = (tile_x[tileline_nth] & mask) >> (7 - pixel_nth); // scan tileline left to rght
          const bool index_0 = (tile_x[tileline_nth + 1] & mask) >> (7 - pixel_nth);

          const std::size_t palette_index = (index_1 << 1) | index_0;

          put_pixel(pixel_nth + x, y, palette[palette_index]);
        }
      }
    }

    End();
  }
}
}
