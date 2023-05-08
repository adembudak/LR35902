#include <CLI/CLI.hpp>
#include <fmt/core.h>

#include <array>
#include <cstdint>
#include <fstream>
#include <string>
#include <vector>

static const std::array<std::string, 256> cb = { //
    "rlc b",    "rlc c",    "rlc d",    "rlc e",    "rlc h",    "rlc l",    "rlc [hl]",    "rlc a",
    "rrc b",    "rrc c",    "rrc d",    "rrc e",    "rrc h",    "rrc l",    "rrc [hl]",    "rrc a",
    "rl b",     "rl c",     "rl d",     "rl e",     "rl h",     "rl l",     "rl [hl]",     "rl a",
    "rr b",     "rr c",     "rr d",     "rr e",     "rr h",     "rr l",     "rr [hl]",     "rr a",
    "sla b",    "sla c",    "sla d",    "sla e",    "sla h",    "sla l",    "sla [hl]",    "sla a",
    "sra b",    "sra c",    "sra d",    "sra e",    "sra h",    "sra l",    "sra [hl]",    "sra a",
    "swap b",   "swap c",   "swap d",   "swap e",   "swap h",   "swap l",   "swap [hl]",   "swap a",
    "srl b",    "srl c",    "srl d",    "srl e",    "srl h",    "srl l",    "srl [hl]",    "srl a",
    "bit 0, b", "bit 0, c", "bit 0, d", "bit 0, e", "bit 0, h", "bit 0, l", "bit 0, [hl]", "bit 0, a",
    "bit 1, b", "bit 1, c", "bit 1, d", "bit 1, e", "bit 1, h", "bit 1, l", "bit 1, [hl]", "bit 1, a",
    "bit 2, b", "bit 2, c", "bit 2, d", "bit 2, e", "bit 2, h", "bit 2, l", "bit 2, [hl]", "bit 2, a",
    "bit 3, b", "bit 3, c", "bit 3, d", "bit 3, e", "bit 3, h", "bit 3, l", "bit 3, [hl]", "bit 3, a",
    "bit 4, b", "bit 4, c", "bit 4, d", "bit 4, e", "bit 4, h", "bit 4, l", "bit 4, [hl]", "bit 4, a",
    "bit 5, b", "bit 5, c", "bit 5, d", "bit 5, e", "bit 5, h", "bit 5, l", "bit 5, [hl]", "bit 5, a",
    "bit 6, b", "bit 6, c", "bit 6, d", "bit 6, e", "bit 6, h", "bit 6, l", "bit 6, [hl]", "bit 6, a",
    "bit 7, b", "bit 7, c", "bit 7, d", "bit 7, e", "bit 7, h", "bit 7, l", "bit 7, [hl]", "bit 7, a",
    "res 0, b", "res 0, c", "res 0, d", "res 0, e", "res 0, h", "res 0, l", "res 0, [hl]", "res 0, a",
    "res 1, b", "res 1, c", "res 1, d", "res 1, e", "res 1, h", "res 1, l", "res 1, [hl]", "res 1, a",
    "res 2, b", "res 2, c", "res 2, d", "res 2, e", "res 2, h", "res 2, l", "res 2, [hl]", "res 2, a",
    "res 3, b", "res 3, c", "res 3, d", "res 3, e", "res 3, h", "res 3, l", "res 3, [hl]", "res 3, a",
    "res 4, b", "res 4, c", "res 4, d", "res 4, e", "res 4, h", "res 4, l", "res 4, [hl]", "res 4, a",
    "res 5, b", "res 5, c", "res 5, d", "res 5, e", "res 5, h", "res 5, l", "res 5, [hl]", "res 5, a",
    "res 6, b", "res 6, c", "res 6, d", "res 6, e", "res 6, h", "res 6, l", "res 6, [hl]", "res 6, a",
    "res 7, b", "res 7, c", "res 7, d", "res 7, e", "res 7, h", "res 7, l", "res 7, [hl]", "res 7, a",
    "set 0, b", "set 0, c", "set 0, d", "set 0, e", "set 0, h", "set 0, l", "set 0, [hl]", "set 0, a",
    "set 1, b", "set 1, c", "set 1, d", "set 1, e", "set 1, h", "set 1, l", "set 1, [hl]", "set 1, a",
    "set 2, b", "set 2, c", "set 2, d", "set 2, e", "set 2, h", "set 2, l", "set 2, [hl]", "set 2, a",
    "set 3, b", "set 3, c", "set 3, d", "set 3, e", "set 3, h", "set 3, l", "set 3, [hl]", "set 3, a",
    "set 4, b", "set 4, c", "set 4, d", "set 4, e", "set 4, h", "set 4, l", "set 4, [hl]", "set 4, a",
    "set 5, b", "set 5, c", "set 5, d", "set 5, e", "set 5, h", "set 5, l", "set 5, [hl]", "set 5, a",
    "set 6, b", "set 6, c", "set 6, d", "set 6, e", "set 6, h", "set 6, l", "set 6, [hl]", "set 6, a",
    "set 7, b", "set 7, c", "set 7, d", "set 7, e", "set 7, h", "set 7, l", "set 7, [hl]", "set 7, a"};

int main(int argc, const char *const argv[]) {
  CLI::App app;

  std::string romFile;
  const auto rom = app.add_option("rom.file.gb", romFile) //
                       ->check(CLI::ExistingFile)
                       ->required(true);

  try {
    app.parse(argc, argv);
  }
  catch(const CLI::ParseError &e) {
    return app.exit(e);
  }

  using byte = std::uint8_t;
  using sbyte = std::int8_t;
  using word = std::uint16_t;
  using offset_t = std::size_t;

  std::ifstream fin{romFile, std::ios_base::in | std::ios_base::binary};
  const std::vector<byte> dumpedROM(std::istreambuf_iterator<char>{fin}, {});
  fin.close();

  const auto fetchOpcode = [&](offset_t &offset) -> byte { return dumpedROM[offset++]; };
  const auto fetchByte = [&](offset_t &offset) -> byte { return dumpedROM[offset++]; };
  const auto fetchsByte = [&](offset_t &offset) -> sbyte { return dumpedROM[offset++]; };

  const auto fetchWord = [&](offset_t &offset) -> word {
    const byte lo = dumpedROM[offset++];
    const byte hi = dumpedROM[offset++];
    return word((hi << 8) | lo);
  };

  const auto disassembleInstruction = [&](byte b, std::size_t &offset) {
    switch(b) {
    case 0x00: return fmt::format("nop");
    case 0x01: return fmt::format("ld bc, ${:02x}", fetchWord(offset));
    case 0x02: return fmt::format("ld [bc], a");
    case 0x03: return fmt::format("inc bc");
    case 0x04: return fmt::format("inc b");
    case 0x05: return fmt::format("dec b");
    case 0x06: return fmt::format("ld b, ${:02x}", fetchByte(offset));
    case 0x07: return fmt::format("rlca");
    case 0x08: return fmt::format("ld [${:02x}], sp", fetchWord(offset));
    case 0x09: return fmt::format("add hl, bc");
    case 0x0a: return fmt::format("ld a, [bc]");
    case 0x0b: return fmt::format("dec bc");
    case 0x0c: return fmt::format("inc c");
    case 0x0d: return fmt::format("dec c");
    case 0x0e: return fmt::format("ld c, ${:02x}", fetchByte(offset));
    case 0x0f: return fmt::format("rrca");
    case 0x10: return fmt::format("stop");
    case 0x11: return fmt::format("ld de, ${:02x}", fetchWord(offset));
    case 0x12: return fmt::format("ld [de], a");
    case 0x13: return fmt::format("inc de");
    case 0x14: return fmt::format("inc d");
    case 0x15: return fmt::format("dec d");
    case 0x16: return fmt::format("ld d, ${:02x}", fetchByte(offset));
    case 0x17: return fmt::format("rla");
    case 0x18: return fmt::format("jr {:d}", fetchsByte(offset));
    case 0x19: return fmt::format("add hl, de");
    case 0x1a: return fmt::format("ld a, [de]");
    case 0x1b: return fmt::format("dec de");
    case 0x1c: return fmt::format("inc e");
    case 0x1d: return fmt::format("dec e");
    case 0x1e: return fmt::format("ld e, ${:02x}", fetchByte(offset));
    case 0x1f: return fmt::format("rra");
    case 0x20: return fmt::format("jr nz, {:d}", fetchsByte(offset));
    case 0x21: return fmt::format("ld hl, ${:02x}", fetchWord(offset));
    case 0x22: return fmt::format("ld [hl+], a");
    case 0x23: return fmt::format("inc hl");
    case 0x24: return fmt::format("inc h");
    case 0x25: return fmt::format("dec h");
    case 0x26: return fmt::format("ld h, ${:02x}", fetchByte(offset));
    case 0x27: return fmt::format("daa");
    case 0x28: return fmt::format("jr z, {:d}", fetchsByte(offset));
    case 0x29: return fmt::format("add hl, hl");
    case 0x2a: return fmt::format("ld a, [hl+]");
    case 0x2b: return fmt::format("dec hl");
    case 0x2c: return fmt::format("inc l");
    case 0x2d: return fmt::format("dec l");
    case 0x2e: return fmt::format("ld l, ${:02x}", fetchByte(offset));
    case 0x2f: return fmt::format("cpl");
    case 0x30: return fmt::format("jr nc, {:d}", fetchsByte(offset));
    case 0x31: return fmt::format("ld sp, ${:02x}", fetchWord(offset));
    case 0x32: return fmt::format("ld [hl-], a");
    case 0x33: return fmt::format("inc sp");
    case 0x34: return fmt::format("inc [hl]");
    case 0x35: return fmt::format("dec [hl]");
    case 0x36: return fmt::format("ld [hl], ${:02x}", fetchByte(offset));
    case 0x37: return fmt::format("scf");
    case 0x38: return fmt::format("jr c, {:d}", fetchsByte(offset));
    case 0x39: return fmt::format("add hl, sp");
    case 0x3a: return fmt::format("ld a, [hl-]");
    case 0x3b: return fmt::format("dec sp");
    case 0x3c: return fmt::format("inc a");
    case 0x3d: return fmt::format("dec a");
    case 0x3e: return fmt::format("ld a, ${:02x}", fetchByte(offset));
    case 0x3f: return fmt::format("ccf");
    case 0x40: return fmt::format("ld b, b");
    case 0x41: return fmt::format("ld b, c");
    case 0x42: return fmt::format("ld b, d");
    case 0x43: return fmt::format("ld b, e");
    case 0x44: return fmt::format("ld b, h");
    case 0x45: return fmt::format("ld b, l");
    case 0x46: return fmt::format("ld b, [hl]");
    case 0x47: return fmt::format("ld b, a");
    case 0x48: return fmt::format("ld c, b");
    case 0x49: return fmt::format("ld c, c");
    case 0x4a: return fmt::format("ld c, d");
    case 0x4b: return fmt::format("ld c, e");
    case 0x4c: return fmt::format("ld c, h");
    case 0x4d: return fmt::format("ld c, l");
    case 0x4e: return fmt::format("ld c, [hl]");
    case 0x4f: return fmt::format("ld c, a");
    case 0x50: return fmt::format("ld d, b");
    case 0x51: return fmt::format("ld d, c");
    case 0x52: return fmt::format("ld d, d");
    case 0x53: return fmt::format("ld d, e");
    case 0x54: return fmt::format("ld d, h");
    case 0x55: return fmt::format("ld d, l");
    case 0x56: return fmt::format("ld d, [hl]");
    case 0x57: return fmt::format("ld d, a");
    case 0x58: return fmt::format("ld e, b");
    case 0x59: return fmt::format("ld e, c");
    case 0x5a: return fmt::format("ld e, d");
    case 0x5b: return fmt::format("ld e, e");
    case 0x5c: return fmt::format("ld e, h");
    case 0x5d: return fmt::format("ld e, l");
    case 0x5e: return fmt::format("ld e, [hl]");
    case 0x5f: return fmt::format("ld e, a");
    case 0x60: return fmt::format("ld h, b");
    case 0x61: return fmt::format("ld h, c");
    case 0x62: return fmt::format("ld h, d");
    case 0x63: return fmt::format("ld h, e");
    case 0x64: return fmt::format("ld h, h");
    case 0x65: return fmt::format("ld h, l");
    case 0x66: return fmt::format("ld h, [hl]");
    case 0x67: return fmt::format("ld h, a");
    case 0x68: return fmt::format("ld l, b");
    case 0x69: return fmt::format("ld l, c");
    case 0x6a: return fmt::format("ld l, d");
    case 0x6b: return fmt::format("ld l, e");
    case 0x6c: return fmt::format("ld l, h");
    case 0x6d: return fmt::format("ld l, l");
    case 0x6e: return fmt::format("ld l, [hl]");
    case 0x6f: return fmt::format("ld l, a");
    case 0x70: return fmt::format("ld [hl], b");
    case 0x71: return fmt::format("ld [hl], c");
    case 0x72: return fmt::format("ld [hl], d");
    case 0x73: return fmt::format("ld [hl], e");
    case 0x74: return fmt::format("ld [hl], h");
    case 0x75: return fmt::format("ld [hl], l");
    case 0x76: return fmt::format("halt");
    case 0x77: return fmt::format("ld [hl], a");
    case 0x78: return fmt::format("ld a, b");
    case 0x79: return fmt::format("ld a, c");
    case 0x7a: return fmt::format("ld a, d");
    case 0x7b: return fmt::format("ld a, e");
    case 0x7c: return fmt::format("ld a, h");
    case 0x7d: return fmt::format("ld a, l");
    case 0x7e: return fmt::format("ld a, [hl]");
    case 0x7f: return fmt::format("ld a, a");
    case 0x80: return fmt::format("add a, b");
    case 0x81: return fmt::format("add a, c");
    case 0x82: return fmt::format("add a, d");
    case 0x83: return fmt::format("add a, e");
    case 0x84: return fmt::format("add a, h");
    case 0x85: return fmt::format("add a, l");
    case 0x86: return fmt::format("add a, [hl]");
    case 0x87: return fmt::format("add a, a");
    case 0x88: return fmt::format("adc a, b");
    case 0x89: return fmt::format("adc a, c");
    case 0x8a: return fmt::format("adc a, d");
    case 0x8b: return fmt::format("adc a, e");
    case 0x8c: return fmt::format("adc a, h");
    case 0x8d: return fmt::format("adc a, l");
    case 0x8e: return fmt::format("adc a, [hl]");
    case 0x8f: return fmt::format("adc a, a");
    case 0x90: return fmt::format("sub a, b");
    case 0x91: return fmt::format("sub a, c");
    case 0x92: return fmt::format("sub a, d");
    case 0x93: return fmt::format("sub a, e");
    case 0x94: return fmt::format("sub a, h");
    case 0x95: return fmt::format("sub a, l");
    case 0x96: return fmt::format("sub a, [hl]");
    case 0x97: return fmt::format("sub a, a");
    case 0x98: return fmt::format("sbc a, b");
    case 0x99: return fmt::format("sbc a, c");
    case 0x9a: return fmt::format("sbc a, d");
    case 0x9b: return fmt::format("sbc a, e");
    case 0x9c: return fmt::format("sbc a, h");
    case 0x9d: return fmt::format("sbc a, l");
    case 0x9e: return fmt::format("sbc a, [hl]");
    case 0x9f: return fmt::format("sbc a, a");
    case 0xa0: return fmt::format("and a, b");
    case 0xa1: return fmt::format("and a, c");
    case 0xa2: return fmt::format("and a, d");
    case 0xa3: return fmt::format("and a, e");
    case 0xa4: return fmt::format("and a, h");
    case 0xa5: return fmt::format("and a, l");
    case 0xa6: return fmt::format("and a, [hl]");
    case 0xa7: return fmt::format("and a, a");
    case 0xa8: return fmt::format("xor a, b");
    case 0xa9: return fmt::format("xor a, c");
    case 0xaa: return fmt::format("xor a, d");
    case 0xab: return fmt::format("xor a, e");
    case 0xac: return fmt::format("xor a, h");
    case 0xad: return fmt::format("xor a, l");
    case 0xae: return fmt::format("xor a, [hl]");
    case 0xaf: return fmt::format("xor a, a");
    case 0xb0: return fmt::format("or a, b");
    case 0xb1: return fmt::format("or a, c");
    case 0xb2: return fmt::format("or a, d");
    case 0xb3: return fmt::format("or a, e");
    case 0xb4: return fmt::format("or a, h");
    case 0xb5: return fmt::format("or a, l");
    case 0xb6: return fmt::format("or a, [hl]");
    case 0xb7: return fmt::format("or a, a");
    case 0xb8: return fmt::format("cp a, b");
    case 0xb9: return fmt::format("cp a, c");
    case 0xba: return fmt::format("cp a, d");
    case 0xbb: return fmt::format("cp a, e");
    case 0xbc: return fmt::format("cp a, h");
    case 0xbd: return fmt::format("cp a, l");
    case 0xbe: return fmt::format("cp a, [hl]");
    case 0xbf: return fmt::format("cp a, a");
    case 0xc0: return fmt::format("ret nz");
    case 0xc1: return fmt::format("pop bc");
    case 0xc2: return fmt::format("jp nz, ${:02x}", fetchWord(offset));
    case 0xc3: return fmt::format("jp ${:02x}", fetchWord(offset));
    case 0xc4: return fmt::format("call nz, ${:02x}", fetchWord(offset));
    case 0xc5: return fmt::format("push bc");
    case 0xc6: return fmt::format("add a, ${:02x}", fetchByte(offset));
    case 0xc7: return fmt::format("rst $00");
    case 0xc8: return fmt::format("ret z");
    case 0xc9: return fmt::format("ret");
    case 0xca: return fmt::format("jp z, ${:02x}", fetchWord(offset));
    case 0xcb: return fmt::format("{}", cb[fetchOpcode(offset)]);
    case 0xcc: return fmt::format("call z, ${:02x}", fetchWord(offset));
    case 0xcd: return fmt::format("call ${:02x}", fetchWord(offset));
    case 0xce: return fmt::format("adc a, ${:02x}", fetchByte(offset));
    case 0xcf: return fmt::format("rst 08h");
    case 0xd0: return fmt::format("ret nc");
    case 0xd1: return fmt::format("pop de");
    case 0xd2: return fmt::format("jp nc, ${:02x}", fetchWord(offset));
    case 0xd3: return fmt::format("unused");
    case 0xd4: return fmt::format("call nc, ${:02x}", fetchWord(offset));
    case 0xd5: return fmt::format("push de");
    case 0xd6: return fmt::format("sub a, ${:02x}", fetchByte(offset));
    case 0xd7: return fmt::format("rst $10");
    case 0xd8: return fmt::format("ret c");
    case 0xd9: return fmt::format("reti");
    case 0xda: return fmt::format("jp c, ${:02x}", fetchWord(offset));
    case 0xdb: return fmt::format("unused");
    case 0xdc: return fmt::format("call c, ${:02x}", fetchWord(offset));
    case 0xdd: return fmt::format("unused");
    case 0xde: return fmt::format("sbc a, ${:02x}", fetchByte(offset));
    case 0xdf: return fmt::format("rst 18h");
    case 0xe0: return fmt::format("ld [$ff00 + ${:02x}], a", fetchByte(offset));
    case 0xe1: return fmt::format("pop hl");
    case 0xe2: return fmt::format("ld [$ff00 + c], a");
    case 0xe3: return fmt::format("unused");
    case 0xe4: return fmt::format("unused");
    case 0xe5: return fmt::format("push hl");
    case 0xe6: return fmt::format("and a, ${:02x}", fetchByte(offset));
    case 0xe7: return fmt::format("rst $20");
    case 0xe8: return fmt::format("add sp, {:d}", fetchsByte(offset));
    case 0xe9: return fmt::format("jp hl");
    case 0xea: return fmt::format("ld [${:02x}], a", fetchWord(offset));
    case 0xeb: return fmt::format("unused");
    case 0xec: return fmt::format("unused");
    case 0xed: return fmt::format("unused");
    case 0xee: return fmt::format("xor a, ${:02x}", fetchByte(offset));
    case 0xef: return fmt::format("rst 28h");
    case 0xf0: return fmt::format("ld a, [$ff00 + ${:02x}]", fetchByte(offset));
    case 0xf1: return fmt::format("pop af");
    case 0xf2: return fmt::format("ld a, [$ff00 + c]");
    case 0xf3: return fmt::format("di");
    case 0xf4: return fmt::format("unused");
    case 0xf5: return fmt::format("push af");
    case 0xf6: return fmt::format("or a, ${:02x}", fetchByte(offset));
    case 0xf7: return fmt::format("rst $30");
    case 0xf8: return fmt::format("ld hl, sp {:+d}", fetchsByte(offset));
    case 0xf9: return fmt::format("ld sp, hl");
    case 0xfa: return fmt::format("ld a, [${:02x}]", fetchWord(offset));
    case 0xfb: return fmt::format("ei");
    case 0xfc: return fmt::format("unused");
    case 0xfd: return fmt::format("unused");
    case 0xfe: return fmt::format("cp a, ${:02x}", fetchByte(offset));
    case 0xff: return fmt::format("rst $38");
    default: return std::string{};
    }
  };

  for(std::size_t offset = 0; offset < dumpedROM.size(); /* */) {
    const auto offset_old = offset;
    const byte opcode = fetchOpcode(offset);
    fmt::print("{0:<20} ; ${1:04x}\n", disassembleInstruction(opcode, offset), offset_old);
  }
}
