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
  _memory_portions_sram = gameboy.cart.SRAMSize().has_value();
}

void DebugView::showCartHeader() noexcept {
  using namespace ImGui;

  if(_header) {
    Begin("Cartridge Header", &_header);

    Text("Title:      : %s\n", gameboy.cart.header.decode_title().c_str());
    Text("Destination : %s\n", gameboy.cart.header.decode_destination_name().c_str());

    Text("CGB Support : %s\n", gameboy.cart.header.decode_cgb_support().c_str());
    Text("SGB Support : %s\n", gameboy.cart.header.decode_sgb_support().c_str());

    Text("Logo Check  : %s\n", gameboy.cart.header.is_logocheck_ok() ? "Pass" : "Fail");
    Text("Checksum    : %s\n", gameboy.cart.header.is_checksum_ok() ? "Pass" : "Fail");

    Text("MBC type    : %s\n", gameboy.cart.header.decode_mbc_type().first.c_str());

    Text("ROM Size    : %s\n", gameboy.cart.header.decode_rom_size().first.c_str());
    Text("RAM Size    : %s\n", gameboy.cart.header.decode_ram_size().first.c_str());

    Text("Publisher   : %s\n", gameboy.cart.header.decode_licensee_name().c_str());
    Text("Version     : %lu\n", gameboy.cart.header.decode_version());

    End();
  }
}

void DebugView::showMemoryPortions() noexcept {
  using namespace ImGui;

  if(_memory_portions) {
    Begin("Memory Portions", &_memory_portions);

    if(static MemoryEditor memory_editor; BeginTabBar("Tab Bar")) {
      memory_editor.ReadOnly = true;

      constexpr std::size_t rom_bank_size = 16_KiB;
      if(BeginTabItem("rom0", &_memory_portions_rom)) {
        memory_editor.DrawContents(std::bit_cast<void *>(gameboy.cart.data()), rom_bank_size, rom0);
        EndTabItem();
      }

      const int number_of_banks = gameboy.cart.size() / rom_bank_size;
      for(int i = 1; i < number_of_banks; ++i) {
        const std::string label = "rom" + std::to_string(i);

        if(BeginTabItem(label.c_str(), &_memory_portions_rom)) {
          memory_editor.DrawContents(std::bit_cast<void *>(gameboy.cart.data() + (i * rom_bank_size)),
                                     rom_bank_size, romx);
          EndTabItem();
        }
      }

      if(BeginTabItem("vram", &_memory_portions_vram)) {
        memory_editor.DrawContents(std::bit_cast<void *>(std::data(gameboy.ppu.m_vram)),
                                   std::size(gameboy.ppu.m_vram), vram);

        EndTabItem();
      }

      if(gameboy.cart.SRAMSize()) {
        constexpr std::size_t sram_bank_size = 8_KiB;
        const int number_of_banks = *gameboy.cart.SRAMSize() / sram_bank_size;

        for(int i = 0; i < number_of_banks; ++i) {
          const std::string label = "sram" + std::to_string(i);

          if(BeginTabItem(label.c_str(), &_memory_portions_sram)) {
            memory_editor.DrawContents(
                std::bit_cast<void *>(gameboy.cart.SRAMData().value() + (i * sram_bank_size)), sram_bank_size,
                sram);

            EndTabItem();
          }
        }
      }

      if(BeginTabItem("wram", &_memory_portions_wram)) {
        memory_editor.DrawContents(std::bit_cast<void *>(std::data(gameboy.builtIn.m_wram)),
                                   std::size(gameboy.builtIn.m_wram), wram0);

        EndTabItem();
      }

      if(BeginTabItem("echo", &_memory_portions_echo)) {
        memory_editor.DrawContents(std::bit_cast<void *>(std::data(gameboy.builtIn.m_echo)),
                                   std::size(gameboy.builtIn.m_echo), echo);

        EndTabItem();
      }

      if(BeginTabItem("oam", &_memory_portions_oam)) {
        memory_editor.DrawContents(std::bit_cast<void *>(std::data(gameboy.ppu.m_oam)),
                                   std::size(gameboy.ppu.m_oam), oam);

        EndTabItem();
      }

      if(BeginTabItem("noUsable", &_memory_portions_noUsable)) {
        memory_editor.DrawContents(std::bit_cast<void *>(std::data(gameboy.builtIn.m_noUsable)),
                                   std::size(gameboy.builtIn.m_noUsable), noUsable);

        EndTabItem();
      }

      if(BeginTabItem("io", &_memory_portions_io)) {
        memory_editor.DrawContents(std::bit_cast<void *>(std::data(gameboy.io.m_data)),
                                   std::size(gameboy.io.m_data), io);

        EndTabItem();
      }

      if(BeginTabItem("hram", &_memory_portions_hram)) {
        memory_editor.DrawContents(std::bit_cast<void *>(std::data(gameboy.builtIn.m_hram)),
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

    if(static std::string label = "Pause"; Button(label.c_str())) {
      gameboy.m_paused ^= true;
      gameboy.m_paused ? label = "Cont." : label = "Pause";
    }

    iv.insert_or_assign(gameboy.cpu.PC.m_data, std::pair{gameboy.cpu.opcode, gameboy.cpu.immediate});

    for(const auto &[PC, op] : iv) {
      ImVec4 color{0.53f, 0.53f, 0.53f, 1.0f};
      if(PC == gameboy.cpu.PC.m_data) color = ImVec4{1.00f, 1.00f, 1.00f, 1.0f};

      const auto [opcode, immediate] = op;
      if(PC != 0)
        if(std::holds_alternative<byte>(immediate))
          TextColored(color, "%04x  %02x %02x", PC - 2, opcode, std::get<byte>(immediate));
        else if(std::holds_alternative<word>(immediate))
          TextColored(color, "%04x  %02x %02x", PC - 3, opcode, std::get<word>(immediate));
        else TextColored(color, "%04x  %02x", PC - 1, opcode);
      else TextColored(color, "%04x  %02x", PC, opcode);
    }

    End();
  }
}

void DebugView::showCPUState() noexcept {
  using namespace ImGui;

  if(_cpu_state) {
    Begin("CPU State", &_cpu_state);
    const auto &cpu = gameboy.cpu;

    Text("Cycles: %lu\nLatest: %lu", cpu.m_clock.m_data, gameboy.clock.m_latest);

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
    const auto &ppu = gameboy.ppu;

    if(Checkbox("LCD", &showLCDRegisters); showLCDRegisters) {
      Text("LCDC: %x\n%s\n%s%s%s\n%s%s%s\n%s%s\n", io.LCDC,
           io.LCDC & 0b1000'0000 ? "LCD enabled\n" : "LCD disabled\n",

           io.LCDC & 0b0010'0000 ? "Win enabled\n" : "Win disabled\n",
           io.LCDC & 0b0001'0000 ? "Win Tileset: 0x8000\n" : "Win Tileset: 0x8800\n",
           io.LCDC & 0b0100'0000 ? "Win Tilemap: 0x9c00\n" : "Win Tilemap: 0x8800\n",

           io.LCDC & 0b0000'0001 ? "Bg enabled\n" : "Bg disabled\n",
           io.LCDC & 0b0001'0000 ? "Bg Tileset: 0x8000\n" : "Bg Tileset: 0x8800\n",
           io.LCDC & 0b0000'1000 ? "Bg Tilemap: 0x9c00\n" : "Bg Tilemap: 0x9800\n",

           io.LCDC & 0b0000'0010 ? "Sprites enabled\n" : "Sprites disabled\n",
           io.LCDC & 0b0000'0100 ? "Sprite size: 16\n" : "Sprite size: 8\n");

      Text("STAT: %x", io.STAT);

      static ImGuiTableFlags flags = ImGuiTableFlags_SizingFixedFit  //
                                     | ImGuiTableFlags_RowBg         //
                                     | ImGuiTableFlags_Borders       //
                                     | ImGuiTableFlags_NoHostExtendX //
                                     | ImGuiTableFlags_Hideable;

      const char *const kind[5]{"", "coincidence", "oam", "vblank", "hblank"};
      if(BeginTable("Stat Interrupts", /*columns*/ 5, flags)) {
        for(int i = 0; i < 5; ++i)
          TableSetupColumn(kind[i]);
        TableHeadersRow();

        TableNextRow();
        TableSetColumnIndex(0);
        Text("%s", "Enabled: ");

        TableSetColumnIndex(1);
        Text("%d", bool(io.STAT & 0b0100'0000));

        TableSetColumnIndex(2);
        Text("%d", bool(io.STAT & 0b0010'0000));

        TableSetColumnIndex(3);
        Text("%d", bool(io.STAT & 0b0001'0000));

        TableSetColumnIndex(4);
        Text("%d", bool(io.STAT & 0b0000'1000));
        ///////////
        TableNextRow();
        TableSetColumnIndex(0);
        Text("%s", "Requested: ");

        TableSetColumnIndex(1);
        Text("%d", io.LY == io.LYC);

        TableSetColumnIndex(2);
        Text("%d", (io.STAT & 0b11) == 0b10);

        TableSetColumnIndex(3);
        Text("%d", (io.STAT & 0b11) == 0b01);

        TableSetColumnIndex(4);
        Text("%d", (io.STAT & 0b11) == 0b00);

        EndTable();
      }

      NewLine();
      Text("SCY: %x", io.SCY);
      Text("SCX: %x", io.SCX);

      NewLine();
      Text("WY: %x", io.WY);
      Text("WX: %x", io.WX);

      NewLine();
      Text("LY:  %u", io.LY);
      Text("LYC: %u", io.LYC);

      NewLine();
      Text("DMA: %x", io.DMA);

      NewLine();
      Text("BGP: %x", io.BGP);
      Text("OBP0: %x", io.OBP0);
      Text("OBP1: %x", io.OBP1);

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
      const auto &IE = gameboy.intr._IE;
      const auto &IF = gameboy.io.IF;

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

constexpr std::size_t tile_size = 16;
constexpr std::size_t tile_w = 8;
constexpr std::size_t tileline_size = 2;
constexpr std::size_t tilemap_size = 2_KiB;

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
    auto put_pixel = [&draw_list](const float x, const float y, const rgba32 &color) {
      ImColor color_{color.r, color.g, color.b};
      const ImVec2 p = ImGui::GetCursorScreenPos();

      draw_list->AddRectFilled({p.x + x, p.y + y}, {p.x + x + 1, p.y + y + 1}, color_);
    };

    using tile = std::array<byte, tile_size>;
    tile tile_x{};
    ///////////////////////

    // iterate tile
    // 0 -> 6144, += 16  (6144 == 6_KiB == tilemap size)
    int x = 0;
    for(std::size_t tile_nth = 0; tile_nth < tilemap_size; tile_nth += tile_size, x += 8) {
      std::copy_n(gameboy.ppu.m_vram.begin() + tile_nth, tile_size, tile_x.begin());
      if(std::all_of(begin(tile_x), end(tile_x), [](byte b) { return b == 0; })) continue;

      int y = 0;
      // iterate tilelines
      // 0 -> 16, += 2
      for(std::size_t tileline_nth = 0; tileline_nth < tile_size; tileline_nth += tileline_size, ++y) {

        byte mask = 0b1000'0000;
        for(std::size_t pixel_nth = 0; pixel_nth < tile_w; ++pixel_nth, mask >>= 1) { // iterates 0->7
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
