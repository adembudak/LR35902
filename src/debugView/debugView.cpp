#include <LR35902/core/core.h>
#include <LR35902/debugView/debugView.h>
#include <LR35902/stubs/builtin/builtin.h>
#include <LR35902/stubs/cartridge/cartridge.h>
#include <LR35902/stubs/io/io.h>
#include <LR35902/stubs/ppu/ppu.h>

#include <imgui/imgui.h>
#include <imgui_memory_editor/imgui_memory_editor.h>

#include <cstdint>

namespace LR35902 {

void DebugView::disassembly(Core &core) noexcept {
  using namespace ImGui;

  Begin("Disassembly");

  std::uint16_t pc_shadow = core.PC.m_data;

  for(; pc_shadow <= 0xffff;) {
  }

  End();
}

bool showCpuRegisters = true;
void DebugView::CPURegisters(const Core &core) noexcept {
  using namespace ImGui;

  Begin("CPU: ", &showCpuRegisters, ImGuiWindowFlags_NoResize);

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

bool showRegisters = true;
bool showJoypadRegister = false;
bool showSerialCableRegisters = false;
bool showTimerRegisters = false;
bool showAudioRegisters = false;
bool showLCDRegistesr = false;

void DebugView::registers(const IO &io) {
  using namespace ImGui;

  Begin("Registers:", &showRegisters);

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

/*
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

void f(const std::uint16_t pc_shadow) {
  using namespace ImGui;

  bool isCBPrefixedInstruction = false;

  switch(pc_shadow) {
  case 0x00: Text("%s", "nop"); break;
  case 0x01: Text("%s", "ld bc, u16"); break;
  case 0x02: Text("%s", "ld [bc], a"); break;
  case 0x03: Text("%s", "inc bc"); break;
  case 0x04: Text("%s", "inc b"); break;
  case 0x05: Text("%s", "dec b"); break;
  case 0x06: Text("%s", "ld b, u8"); break;
  case 0x07: Text("%s", "rlca"); break;
  case 0x08: Text("%s", "ld [u16], sp"); break;
  case 0x09: Text("%s", "add hl, bc"); break;
  case 0x0a: Text("%s", "ld a, [bc]"); break;
  case 0x0b: Text("%s", "dec bc"); break;
  case 0x0c: Text("%s", "inc c"); break;
  case 0x0d: Text("%s", "dec c"); break;
  case 0x0e: Text("%s", "ld c, u8"); break;
  case 0x0f: Text("%s", "rrca"); break;
  case 0x10: Text("%s", "stop"); break;
  case 0x11: Text("%s", "ld de, u16"); break;
  case 0x12: Text("%s", "ld(de), a"); break;
  case 0x13: Text("%s", "inc de"); break;
  case 0x14: Text("%s", "inc d"); break;
  case 0x15: Text("%s", "dec d"); break;
  case 0x16: Text("%s", "ld d, u8"); break;
  case 0x17: Text("%s", "rla"); break;
  case 0x18: Text("%s", "jr i8"); break;
  case 0x19: Text("%s", "add hl, de"); break;
  case 0x1a: Text("%s", "ld a, (de)"); break;
  case 0x1b: Text("%s", "dec de"); break;
  case 0x1c: Text("%s", "inc e"); break;
  case 0x1d: Text("%s", "dec e"); break;
  case 0x1e: Text("%s", "ld e, u8"); break;
  case 0x1f: Text("%s", "rra"); break;
  case 0x20: Text("%s", "jr nz, i8"); break;
  case 0x21: Text("%s", "ld hl, u16"); break;
  case 0x22: Text("%s", "ld(hl +), a"); break;
  case 0x23: Text("%s", "inc hl"); break;
  case 0x24: Text("%s", "inc h"); break;
  case 0x25: Text("%s", "dec h"); break;
  case 0x26: Text("%s", "ld h, u8"); break;
  case 0x27: Text("%s", "daa"); break;
  case 0x28: Text("%s", "jr z, i8"); break;
  case 0x29: Text("%s", "add hl, hl"); break;
  case 0x2a: Text("%s", "ld a, (hl+)"); break;
  case 0x2b: Text("%s", "dec hl"); break;
  case 0x2c: Text("%s", "inc l"); break;
  case 0x2d: Text("%s", "dec l"); break;
  case 0x2e: Text("%s", "ld l, u8"); break;
  case 0x2f: Text("%s", "cpl"); break;
  case 0x30: Text("%s", "jr nc, i8"); break;
  case 0x31: Text("%s", "ld sp, u16"); break;
  case 0x32: Text("%s", "ld(hl -), a"); break;
  case 0x33: Text("%s", "inc sp"); break;
  case 0x34: Text("%s", "inc[hl]"); break;
  case 0x35: Text("%s", "dec[hl]"); break;
  case 0x36: Text("%s", "ld[hl], u8"); break;
  case 0x37: Text("%s", "scf"); break;
  case 0x38: Text("%s", "jr c, i8"); break;
  case 0x39: Text("%s", "add hl, sp"); break;
  case 0x3a: Text("%s", "ld a, (hl -)"); break;
  case 0x3b: Text("%s", "dec sp"); break;
  case 0x3c: Text("%s", "inc a"); break;
  case 0x3d: Text("%s", "dec a"); break;
  case 0x3e: Text("%s", "ld a, u8"); break;
  case 0x3f: Text("%s", "ccf"); break;
  case 0x40: Text("%s", "ld b, b"); break;
  case 0x41: Text("%s", "ld b, c"); break;
  case 0x42: Text("%s", "ld b, d"); break;
  case 0x43: Text("%s", "ld b, e"); break;
  case 0x44: Text("%s", "ld b, h"); break;
  case 0x45: Text("%s", "ld b, l"); break;
  case 0x46: Text("%s", "ld b, [hl]"); break;
  case 0x47: Text("%s", "ld b, a"); break;
  case 0x48: Text("%s", "ld c, b"); break;
  case 0x49: Text("%s", "ld c, c"); break;
  case 0x4a: Text("%s", "ld c, d"); break;
  case 0x4b: Text("%s", "ld c, e"); break;
  case 0x4c: Text("%s", "ld c, h"); break;
  case 0x4d: Text("%s", "ld c, l"); break;
  case 0x4e: Text("%s", "ld c, [hl]"); break;
  case 0x4f: Text("%s", "ld c, a"); break;
  case 0x50: Text("%s", "ld d, b"); break;
  case 0x51: Text("%s", "ld d, c"); break;
  case 0x52: Text("%s", "ld d, d"); break;
  case 0x53: Text("%s", "ld d, e"); break;
  case 0x54: Text("%s", "ld d, h"); break;
  case 0x55: Text("%s", "ld d, l"); break;
  case 0x56: Text("%s", "ld d, [hl]"); break;
  case 0x57: Text("%s", "ld d, a"); break;
  case 0x58: Text("%s", "ld e, b"); break;
  case 0x59: Text("%s", "ld e, c"); break;
  case 0x5a: Text("%s", "ld e, d"); break;
  case 0x5b: Text("%s", "ld e, e"); break;
  case 0x5c: Text("%s", "ld e, h"); break;
  case 0x5d: Text("%s", "ld e, l"); break;
  case 0x5e: Text("%s", "ld e, [hl]"); break;
  case 0x5f: Text("%s", "ld e, a"); break;
  case 0x60: Text("%s", "ld h, b"); break;
  case 0x61: Text("%s", "ld h, c"); break;
  case 0x62: Text("%s", "ld h, d"); break;
  case 0x63: Text("%s", "ld h, e"); break;
  case 0x64: Text("%s", "ld h, h"); break;
  case 0x65: Text("%s", "ld h, l"); break;
  case 0x66: Text("%s", "ld h, [hl]"); break;
  case 0x67: Text("%s", "ld h, a"); break;
  case 0x68: Text("%s", "ld l, b"); break;
  case 0x69: Text("%s", "ld l, c"); break;
  case 0x6a: Text("%s", "ld l, d"); break;
  case 0x6b: Text("%s", "ld l, e"); break;
  case 0x6c: Text("%s", "ld l, h"); break;
  case 0x6d: Text("%s", "ld l, l"); break;
  case 0x6e: Text("%s", "ld l, [hl]"); break;
  case 0x6f: Text("%s", "ld l, a"); break;
  case 0x70: Text("%s", "ld[hl], b"); break;
  case 0x71: Text("%s", "ld[hl], c"); break;
  case 0x72: Text("%s", "ld[hl], d"); break;
  case 0x73: Text("%s", "ld[hl], e"); break;
  case 0x74: Text("%s", "ld[hl], h"); break;
  case 0x75: Text("%s", "ld[hl], l"); break;
  case 0x76: Text("%s", "halt"); break;
  case 0x77: Text("%s", "ld[hl], a"); break;
  case 0x78: Text("%s", "ld a, b"); break;
  case 0x79: Text("%s", "ld a, c"); break;
  case 0x7a: Text("%s", "ld a, d"); break;
  case 0x7b: Text("%s", "ld a, e"); break;
  case 0x7c: Text("%s", "ld a, h"); break;
  case 0x7d: Text("%s", "ld a, l"); break;
  case 0x7e: Text("%s", "ld a, [hl]"); break;
  case 0x7f: Text("%s", "ld a, a"); break;
  case 0x80: Text("%s", "add a, b"); break;
  case 0x81: Text("%s", "add a, c"); break;
  case 0x82: Text("%s", "add a, d"); break;
  case 0x83: Text("%s", "add a, e"); break;
  case 0x84: Text("%s", "add a, h"); break;
  case 0x85: Text("%s", "add a, l"); break;
  case 0x86: Text("%s", "add a, [hl]"); break;
  case 0x87: Text("%s", "add a, a"); break;
  case 0x88: Text("%s", "adc a, b"); break;
  case 0x89: Text("%s", "adc a, c"); break;
  case 0x8a: Text("%s", "adc a, d"); break;
  case 0x8b: Text("%s", "adc a, e"); break;
  case 0x8c: Text("%s", "adc a, h"); break;
  case 0x8d: Text("%s", "adc a, l"); break;
  case 0x8e: Text("%s", "adc a, [hl]"); break;
  case 0x8f: Text("%s", "adc a, a"); break;
  case 0x90: Text("%s", "sub a, b"); break;
  case 0x91: Text("%s", "sub a, c"); break;
  case 0x92: Text("%s", "sub a, d"); break;
  case 0x93: Text("%s", "sub a, e"); break;
  case 0x94: Text("%s", "sub a, h"); break;
  case 0x95: Text("%s", "sub a, l"); break;
  case 0x96: Text("%s", "sub a, [hl]"); break;
  case 0x97: Text("%s", "sub a, a"); break;
  case 0x98: Text("%s", "sbc a, b"); break;
  case 0x99: Text("%s", "sbc a, c"); break;
  case 0x9a: Text("%s", "sbc a, d"); break;
  case 0x9b: Text("%s", "sbc a, e"); break;
  case 0x9c: Text("%s", "sbc a, h"); break;
  case 0x9d: Text("%s", "sbc a, l"); break;
  case 0x9e: Text("%s", "sbc a, [hl]"); break;
  case 0x9f: Text("%s", "sbc a, a"); break;
  case 0xa0: Text("%s", "and a, b"); break;
  case 0xa1: Text("%s", "and a, c"); break;
  case 0xa2: Text("%s", "and a, d"); break;
  case 0xa3: Text("%s", "and a, e"); break;
  case 0xa4: Text("%s", "and a, h"); break;
  case 0xa5: Text("%s", "and a, l"); break;
  case 0xa6: Text("%s", "and a, [hl]"); break;
  case 0xa7: Text("%s", "and a, a"); break;
  case 0xa8: Text("%s", "xor a, b"); break;
  case 0xa9: Text("%s", "xor a, c"); break;
  case 0xaa: Text("%s", "xor a, d"); break;
  case 0xab: Text("%s", "xor a, e"); break;
  case 0xac: Text("%s", "xor a, h"); break;
  case 0xad: Text("%s", "xor a, l"); break;
  case 0xae: Text("%s", "xor a, [hl]"); break;
  case 0xaf: Text("%s", "xor a, a"); break;
  case 0xb0: Text("%s", "or a, b"); break;
  case 0xb1: Text("%s", "or a, c"); break;
  case 0xb2: Text("%s", "or a, d"); break;
  case 0xb3: Text("%s", "or a, e"); break;
  case 0xb4: Text("%s", "or a, h"); break;
  case 0xb5: Text("%s", "or a, l"); break;
  case 0xb6: Text("%s", "or a, [hl]"); break;
  case 0xb7: Text("%s", "or a, a"); break;
  case 0xb8: Text("%s", "cp a, b"); break;
  case 0xb9: Text("%s", "cp a, c"); break;
  case 0xba: Text("%s", "cp a, d"); break;
  case 0xbb: Text("%s", "cp a, e"); break;
  case 0xbc: Text("%s", "cp a, h"); break;
  case 0xbd: Text("%s", "cp a, l"); break;
  case 0xbe: Text("%s", "cp a, [hl]"); break;
  case 0xbf: Text("%s", "cp a, a"); break;
  case 0xc0: Text("%s", "ret nz"); break;
  case 0xc1: Text("%s", "pop bc"); break;
  case 0xc2: Text("%s", "jp nz, u16"); break;
  case 0xc3: Text("%s", "jp u16"); break;
  case 0xc4: Text("%s", "call nz, u16"); break;
  case 0xc5: Text("%s", "push bc"); break;
  case 0xc6: Text("%s", "add a, u8"); break;
  case 0xc7: Text("%s", "rst 00h"); break;
  case 0xc8: Text("%s", "ret z"); break;
  case 0xc9: Text("%s", "ret"); break;
  case 0xca: Text("%s", "jp z, u16"); break;
  case 0xcb: isCBPrefixedInstruction = true; break;
  case 0xcc: Text("%s", "call z, u16"); break;
  case 0xcd: Text("%s", "call u16"); break;
  case 0xce: Text("%s", "adc a, u8"); break;
  case 0xcf: Text("%s", "rst 08h"); break;
  case 0xd0: Text("%s", "ret nc"); break;
  case 0xd1: Text("%s", "pop de"); break;
  case 0xd2: Text("%s", "jp nc, u16"); break;
  case 0xd3: Text("%s", "unused"); break;
  case 0xd4: Text("%s", "call nc, u16"); break;
  case 0xd5: Text("%s", "push de"); break;
  case 0xd6: Text("%s", "sub a, u8"); break;
  case 0xd7: Text("%s", "rst 10h"); break;
  case 0xd8: Text("%s", "ret c"); break;
  case 0xd9: Text("%s", "reti"); break;
  case 0xda: Text("%s", "jp c, u16"); break;
  case 0xdb: Text("%s", "unused"); break;
  case 0xdc: Text("%s", "call c, u16"); break;
  case 0xdd: Text("%s", "unused"); break;
  case 0xde: Text("%s", "sbc a, u8"); break;
  case 0xdf: Text("%s", "rst 18h"); break;
  case 0xe0: Text("%s", "ld(ff00 + u8), a"); break;
  case 0xe1: Text("%s", "pop hl"); break;
  case 0xe2: Text("%s", "ld(ff00 + c), a"); break;
  case 0xe3: Text("%s", "unused"); break;
  case 0xe4: Text("%s", "unused"); break;
  case 0xe5: Text("%s", "push hl"); break;
  case 0xe6: Text("%s", "and a, u8"); break;
  case 0xe7: Text("%s", "rst 20h"); break;
  case 0xe8: Text("%s", "add sp, i8"); break;
  case 0xe9: Text("%s", "jp hl"); break;
  case 0xea: Text("%s", "ld(u16), a"); break;
  case 0xeb: Text("%s", "unused"); break;
  case 0xec: Text("%s", "unused"); break;
  case 0xed: Text("%s", "unused"); break;
  case 0xee: Text("%s", "xor a, u8"); break;
  case 0xef: Text("%s", "rst 28h"); break;
  case 0xf0: Text("%s", "ld a, (ff00 + u8)"); break;
  case 0xf1: Text("%s", "pop af"); break;
  case 0xf2: Text("%s", "ld a, (ff00 + c)"); break;
  case 0xf3: Text("%s", "di"); break;
  case 0xf4: Text("%s", "unused"); break;
  case 0xf5: Text("%s", "push af"); break;
  case 0xf6: Text("%s", "or a, u8"); break;
  case 0xf7: Text("%s", "rst 30h"); break;
  case 0xf8: Text("%s", "ld hl, sp + i8"); break;
  case 0xf9: Text("%s", "ld sp, hl"); break;
  case 0xfa: Text("%s", "ld a, (u16)"); break;
  case 0xfb: Text("%s", "ei"); break;
  case 0xfc: Text("%s", "unused"); break;
  case 0xfd: Text("%s", "unused"); break;
  case 0xfe: Text("%s", "cp a, u8"); break;
  case 0xff: Text("%s", "rst 38h"); break;
  }

  if(isCBPrefixedInstruction) {
    isCBPrefixedInstruction = false;

    switch(pc_shadow) {
    case 0x0: Text("%s", "rlc b"); break;
    case 0x1: Text("%s", "rlc c"); break;
    case 0x2: Text("%s", "rlc d"); break;
    case 0x3: Text("%s", "rlc e"); break;
    case 0x4: Text("%s", "rlc h"); break;
    case 0x5: Text("%s", "rlc l"); break;
    case 0x6: Text("%s", "rlc [hl]"); break;
    case 0x7: Text("%s", "rlc a"); break;
    case 0x8: Text("%s", "rrc b"); break;
    case 0x9: Text("%s", "rrc c"); break;
    case 0xa: Text("%s", "rrc d"); break;
    case 0xb: Text("%s", "rrc e"); break;
    case 0xc: Text("%s", "rrc h"); break;
    case 0xd: Text("%s", "rrc l"); break;
    case 0xe: Text("%s", "rrc [hl]"); break;
    case 0xf: Text("%s", "rrc a"); break;
    case 0x10: Text("%s", "rl b"); break;
    case 0x11: Text("%s", "rl c"); break;
    case 0x12: Text("%s", "rl d"); break;
    case 0x13: Text("%s", "rl e"); break;
    case 0x14: Text("%s", "rl h"); break;
    case 0x15: Text("%s", "rl l"); break;
    case 0x16: Text("%s", "rl [hl]"); break;
    case 0x17: Text("%s", "rl a"); break;
    case 0x18: Text("%s", "rr b"); break;
    case 0x19: Text("%s", "rr c"); break;
    case 0x1a: Text("%s", "rr d"); break;
    case 0x1b: Text("%s", "rr e"); break;
    case 0x1c: Text("%s", "rr h"); break;
    case 0x1d: Text("%s", "rr l"); break;
    case 0x1e: Text("%s", "rr [hl]"); break;
    case 0x1f: Text("%s", "rr a"); break;
    case 0x20: Text("%s", "sla b"); break;
    case 0x21: Text("%s", "sla c"); break;
    case 0x22: Text("%s", "sla d"); break;
    case 0x23: Text("%s", "sla e"); break;
    case 0x24: Text("%s", "sla h"); break;
    case 0x25: Text("%s", "sla l"); break;
    case 0x26: Text("%s", "sla [hl]"); break;
    case 0x27: Text("%s", "sla a"); break;
    case 0x28: Text("%s", "sra b"); break;
    case 0x29: Text("%s", "sra c"); break;
    case 0x2a: Text("%s", "sra d"); break;
    case 0x2b: Text("%s", "sra e"); break;
    case 0x2c: Text("%s", "sra h"); break;
    case 0x2d: Text("%s", "sra l"); break;
    case 0x2e: Text("%s", "sra [hl]"); break;
    case 0x2f: Text("%s", "sra a"); break;
    case 0x30: Text("%s", "swap b"); break;
    case 0x31: Text("%s", "swap c"); break;
    case 0x32: Text("%s", "swap d"); break;
    case 0x33: Text("%s", "swap e"); break;
    case 0x34: Text("%s", "swap h"); break;
    case 0x35: Text("%s", "swap l"); break;
    case 0x36: Text("%s", "swap [hl]"); break;
    case 0x37: Text("%s", "swap a"); break;
    case 0x38: Text("%s", "srl b"); break;
    case 0x39: Text("%s", "srl c"); break;
    case 0x3a: Text("%s", "srl d"); break;
    case 0x3b: Text("%s", "srl e"); break;
    case 0x3c: Text("%s", "srl h"); break;
    case 0x3d: Text("%s", "srl l"); break;
    case 0x3e: Text("%s", "srl [hl]"); break;
    case 0x3f: Text("%s", "srl a"); break;
    case 0x40: Text("%s", "bit 0, b"); break;
    case 0x41: Text("%s", "bit 0, c"); break;
    case 0x42: Text("%s", "bit 0, d"); break;
    case 0x43: Text("%s", "bit 0, e"); break;
    case 0x44: Text("%s", "bit 0, h"); break;
    case 0x45: Text("%s", "bit 0, l"); break;
    case 0x46: Text("%s", "bit 0, [hl]"); break;
    case 0x47: Text("%s", "bit 0, a"); break;
    case 0x48: Text("%s", "bit 1, b"); break;
    case 0x49: Text("%s", "bit 1, c"); break;
    case 0x4a: Text("%s", "bit 1, d"); break;
    case 0x4b: Text("%s", "bit 1, e"); break;
    case 0x4c: Text("%s", "bit 1, h"); break;
    case 0x4d: Text("%s", "bit 1, l"); break;
    case 0x4e: Text("%s", "bit 1, [hl]"); break;
    case 0x4f: Text("%s", "bit 1, a"); break;
    case 0x50: Text("%s", "bit 2, b"); break;
    case 0x51: Text("%s", "bit 2, c"); break;
    case 0x52: Text("%s", "bit 2, d"); break;
    case 0x53: Text("%s", "bit 2, e"); break;
    case 0x54: Text("%s", "bit 2, h"); break;
    case 0x55: Text("%s", "bit 2, l"); break;
    case 0x56: Text("%s", "bit 2, [hl]"); break;
    case 0x57: Text("%s", "bit 2, a"); break;
    case 0x58: Text("%s", "bit 3, b"); break;
    case 0x59: Text("%s", "bit 3, c"); break;
    case 0x5a: Text("%s", "bit 3, d"); break;
    case 0x5b: Text("%s", "bit 3, e"); break;
    case 0x5c: Text("%s", "bit 3, h"); break;
    case 0x5d: Text("%s", "bit 3, l"); break;
    case 0x5e: Text("%s", "bit 3, [hl]"); break;
    case 0x5f: Text("%s", "bit 3, a"); break;
    case 0x60: Text("%s", "bit 4, b"); break;
    case 0x61: Text("%s", "bit 4, c"); break;
    case 0x62: Text("%s", "bit 4, d"); break;
    case 0x63: Text("%s", "bit 4, e"); break;
    case 0x64: Text("%s", "bit 4, h"); break;
    case 0x65: Text("%s", "bit 4, l"); break;
    case 0x66: Text("%s", "bit 4, [hl]"); break;
    case 0x67: Text("%s", "bit 4, a"); break;
    case 0x68: Text("%s", "bit 5, b"); break;
    case 0x69: Text("%s", "bit 5, c"); break;
    case 0x6a: Text("%s", "bit 5, d"); break;
    case 0x6b: Text("%s", "bit 5, e"); break;
    case 0x6c: Text("%s", "bit 5, h"); break;
    case 0x6d: Text("%s", "bit 5, l"); break;
    case 0x6e: Text("%s", "bit 5, [hl]"); break;
    case 0x6f: Text("%s", "bit 5, a"); break;
    case 0x70: Text("%s", "bit 6, b"); break;
    case 0x71: Text("%s", "bit 6, c"); break;
    case 0x72: Text("%s", "bit 6, d"); break;
    case 0x73: Text("%s", "bit 6, e"); break;
    case 0x74: Text("%s", "bit 6, h"); break;
    case 0x75: Text("%s", "bit 6, l"); break;
    case 0x76: Text("%s", "bit 6, [hl]"); break;
    case 0x77: Text("%s", "bit 6, a"); break;
    case 0x78: Text("%s", "bit 7, b"); break;
    case 0x79: Text("%s", "bit 7, c"); break;
    case 0x7a: Text("%s", "bit 7, d"); break;
    case 0x7b: Text("%s", "bit 7, e"); break;
    case 0x7c: Text("%s", "bit 7, h"); break;
    case 0x7d: Text("%s", "bit 7, l"); break;
    case 0x7e: Text("%s", "bit 7, [hl]"); break;
    case 0x7f: Text("%s", "bit 7, a"); break;
    case 0x80: Text("%s", "res 0, b"); break;
    case 0x81: Text("%s", "res 0, c"); break;
    case 0x82: Text("%s", "res 0, d"); break;
    case 0x83: Text("%s", "res 0, e"); break;
    case 0x84: Text("%s", "res 0, h"); break;
    case 0x85: Text("%s", "res 0, l"); break;
    case 0x86: Text("%s", "res 0, [hl]"); break;
    case 0x87: Text("%s", "res 0, a"); break;
    case 0x88: Text("%s", "res 1, b"); break;
    case 0x89: Text("%s", "res 1, c"); break;
    case 0x8a: Text("%s", "res 1, d"); break;
    case 0x8b: Text("%s", "res 1, e"); break;
    case 0x8c: Text("%s", "res 1, h"); break;
    case 0x8d: Text("%s", "res 1, l"); break;
    case 0x8e: Text("%s", "res 1, [hl]"); break;
    case 0x8f: Text("%s", "res 1, a"); break;
    case 0x90: Text("%s", "res 2, b"); break;
    case 0x91: Text("%s", "res 2, c"); break;
    case 0x92: Text("%s", "res 2, d"); break;
    case 0x93: Text("%s", "res 2, e"); break;
    case 0x94: Text("%s", "res 2, h"); break;
    case 0x95: Text("%s", "res 2, l"); break;
    case 0x96: Text("%s", "res 2, [hl]"); break;
    case 0x97: Text("%s", "res 2, a"); break;
    case 0x98: Text("%s", "res 3, b"); break;
    case 0x99: Text("%s", "res 3, c"); break;
    case 0x9a: Text("%s", "res 3, d"); break;
    case 0x9b: Text("%s", "res 3, e"); break;
    case 0x9c: Text("%s", "res 3, h"); break;
    case 0x9d: Text("%s", "res 3, l"); break;
    case 0x9e: Text("%s", "res 3, [hl]"); break;
    case 0x9f: Text("%s", "res 3, a"); break;
    case 0xa0: Text("%s", "res 4, b"); break;
    case 0xa1: Text("%s", "res 4, c"); break;
    case 0xa2: Text("%s", "res 4, d"); break;
    case 0xa3: Text("%s", "res 4, e"); break;
    case 0xa4: Text("%s", "res 4, h"); break;
    case 0xa5: Text("%s", "res 4, l"); break;
    case 0xa6: Text("%s", "res 4, [hl]"); break;
    case 0xa7: Text("%s", "res 4, a"); break;
    case 0xa8: Text("%s", "res 5, b"); break;
    case 0xa9: Text("%s", "res 5, c"); break;
    case 0xaa: Text("%s", "res 5, d"); break;
    case 0xab: Text("%s", "res 5, e"); break;
    case 0xac: Text("%s", "res 5, h"); break;
    case 0xad: Text("%s", "res 5, l"); break;
    case 0xae: Text("%s", "res 5, [hl]"); break;
    case 0xaf: Text("%s", "res 5, a"); break;
    case 0xb0: Text("%s", "res 6, b"); break;
    case 0xb1: Text("%s", "res 6, c"); break;
    case 0xb2: Text("%s", "res 6, d"); break;
    case 0xb3: Text("%s", "res 6, e"); break;
    case 0xb4: Text("%s", "res 6, h"); break;
    case 0xb5: Text("%s", "res 6, l"); break;
    case 0xb6: Text("%s", "res 6, [hl]"); break;
    case 0xb7: Text("%s", "res 6, a"); break;
    case 0xb8: Text("%s", "res 7, b"); break;
    case 0xb9: Text("%s", "res 7, c"); break;
    case 0xba: Text("%s", "res 7, d"); break;
    case 0xbb: Text("%s", "res 7, e"); break;
    case 0xbc: Text("%s", "res 7, h"); break;
    case 0xbd: Text("%s", "res 7, l"); break;
    case 0xbe: Text("%s", "res 7, [hl]"); break;
    case 0xbf: Text("%s", "res 7, a"); break;
    case 0xc0: Text("%s", "set 0, b"); break;
    case 0xc1: Text("%s", "set 0, c"); break;
    case 0xc2: Text("%s", "set 0, d"); break;
    case 0xc3: Text("%s", "set 0, e"); break;
    case 0xc4: Text("%s", "set 0, h"); break;
    case 0xc5: Text("%s", "set 0, l"); break;
    case 0xc6: Text("%s", "set 0, [hl]"); break;
    case 0xc7: Text("%s", "set 0, a"); break;
    case 0xc8: Text("%s", "set 1, b"); break;
    case 0xc9: Text("%s", "set 1, c"); break;
    case 0xca: Text("%s", "set 1, d"); break;
    case 0xcb: Text("%s", "set 1, e"); break;
    case 0xcc: Text("%s", "set 1, h"); break;
    case 0xcd: Text("%s", "set 1, l"); break;
    case 0xce: Text("%s", "set 1, [hl]"); break;
    case 0xcf: Text("%s", "set 1, a"); break;
    case 0xd0: Text("%s", "set 2, b"); break;
    case 0xd1: Text("%s", "set 2, c"); break;
    case 0xd2: Text("%s", "set 2, d"); break;
    case 0xd3: Text("%s", "set 2, e"); break;
    case 0xd4: Text("%s", "set 2, h"); break;
    case 0xd5: Text("%s", "set 2, l"); break;
    case 0xd6: Text("%s", "set 2, [hl]"); break;
    case 0xd7: Text("%s", "set 2, a"); break;
    case 0xd8: Text("%s", "set 3, b"); break;
    case 0xd9: Text("%s", "set 3, c"); break;
    case 0xda: Text("%s", "set 3, d"); break;
    case 0xdb: Text("%s", "set 3, e"); break;
    case 0xdc: Text("%s", "set 3, h"); break;
    case 0xdd: Text("%s", "set 3, l"); break;
    case 0xde: Text("%s", "set 3, [hl]"); break;
    case 0xdf: Text("%s", "set 3, a"); break;
    case 0xe0: Text("%s", "set 4, b"); break;
    case 0xe1: Text("%s", "set 4, c"); break;
    case 0xe2: Text("%s", "set 4, d"); break;
    case 0xe3: Text("%s", "set 4, e"); break;
    case 0xe4: Text("%s", "set 4, h"); break;
    case 0xe5: Text("%s", "set 4, l"); break;
    case 0xe6: Text("%s", "set 4, [hl]"); break;
    case 0xe7: Text("%s", "set 4, a"); break;
    case 0xe8: Text("%s", "set 5, b"); break;
    case 0xe9: Text("%s", "set 5, c"); break;
    case 0xea: Text("%s", "set 5, d"); break;
    case 0xeb: Text("%s", "set 5, e"); break;
    case 0xec: Text("%s", "set 5, h"); break;
    case 0xed: Text("%s", "set 5, l"); break;
    case 0xee: Text("%s", "set 5, [hl]"); break;
    case 0xef: Text("%s", "set 5, a"); break;
    case 0xf0: Text("%s", "set 6, b"); break;
    case 0xf1: Text("%s", "set 6, c"); break;
    case 0xf2: Text("%s", "set 6, d"); break;
    case 0xf3: Text("%s", "set 6, e"); break;
    case 0xf4: Text("%s", "set 6, h"); break;
    case 0xf5: Text("%s", "set 6, l"); break;
    case 0xf6: Text("%s", "set 6, [hl]"); break;
    case 0xf7: Text("%s", "set 6, a"); break;
    case 0xf8: Text("%s", "set 7, b"); break;
    case 0xf9: Text("%s", "set 7, c"); break;
    case 0xfa: Text("%s", "set 7, d"); break;
    case 0xfb: Text("%s", "set 7, e"); break;
    case 0xfc: Text("%s", "set 7, h"); break;
    case 0xfd: Text("%s", "set 7, l"); break;
    case 0xfe: Text("%s", "set 7, [hl]"); break;
    case 0xff: Text("%s", "set 7, a"); break;
    }
  }
}
*/

}
