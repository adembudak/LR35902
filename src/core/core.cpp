#include <LR35902/core/core.h>
#include <LR35902/stubs/bus/bus.h>

#include <cstdint>

namespace LR35902 {

auto Core::fetchOpcode() noexcept -> byte {
  this->opcode = m_bus.read(PC++);
  return opcode;
}

auto Core::fetchByte() noexcept -> byte {
  return m_bus.read(PC++);
};

auto Core::fetchWord() noexcept -> word {
  const byte hi = m_bus.read(PC++);
  const byte lo = m_bus.read(PC++);

  return word(hi << 8 | lo);
};

// opcode table generated from: https://github.com/izik1/gbops/blob/master/dmgops.json
void Core::run() noexcept {

  switch(fetchOpcode()) {
  case 0x00: nop(); break;
  case 0x01: ld(BC, n16{fetchWord()}); break;
  case 0x02: ld(*BC, register_to_memory{}); break;
  case 0x03: inc(BC); break;
  case 0x04: inc(B); break;
  case 0x05: dec(B); break;
  case 0x06: ld(B, n8{fetchByte()}); break;
  case 0x07: rlca(); break;
  case 0x08: {
    const n16 nn{fetchWord()};
    byte &b = m_bus.read_write(nn.m_data);
    ld(b, SP_register_tag{});
    break;
  }
  case 0x09: add(HL_register_tag{}, BC); break;
  case 0x0a: ld(memory_to_register{}, *BC); break;
  case 0x0b: dec(BC); break;
  case 0x0c: inc(C); break;
  case 0x0d: dec(C); break;
  case 0x0e: ld(C, n8{fetchByte()}); break;
  case 0x0f: rrca(); break;
  case 0x10: stop(); break;
  case 0x11: ld(DE, n16{fetchWord()}); break;
  case 0x12: ld(*DE, register_to_memory{}); break;
  case 0x13: inc(DE); break;
  case 0x14: inc(D); break;
  case 0x15: dec(D); break;
  case 0x16: ld(D, n8{fetchByte()}); break;
  case 0x17: rla(); break;
  case 0x18: jr(e8{static_cast<int8_t>(fetchByte())}); break;
  case 0x19: add(HL_register_tag{}, DE); break;
  case 0x1a: ld(memory_to_register{}, *DE); break;
  case 0x1b: dec(DE); break;
  case 0x1c: inc(E); break;
  case 0x1d: dec(E); break;
  case 0x1e: ld(E, n8{fetchByte()}); break;
  case 0x1f: rra(); break;
  case 0x20: jr(cc::nz, e8{static_cast<int8_t>(fetchByte())}); break;
  case 0x21: ld(HL, n16{fetchWord()}); break;
  case 0x22: ld(HLi_tag{}, register_to_memory{}); break;
  case 0x23: inc(HL); break;
  case 0x24: inc(H); break;
  case 0x25: dec(H); break;
  case 0x26: ld(H, n8{fetchByte()}); break;
  case 0x27: daa(); break;
  case 0x28: jr(cc::z, e8{static_cast<int8_t>(fetchByte())}); break;
  case 0x29: add(HL_register_tag{}, HL); break;
  case 0x2a: ld(memory_to_register{}, HLi_tag{}); break;
  case 0x2b: dec(HL); break;
  case 0x2c: inc(L); break;
  case 0x2d: dec(L); break;
  case 0x2e: ld(L, n8{fetchByte()}); break;
  case 0x2f: cpl(); break;
  case 0x30: jr(cc::nc, e8{static_cast<int8_t>(fetchByte())}); break;
  case 0x31: ld(SP_register_tag{}, n16{fetchWord()}); break;
  case 0x32: ld(HLd_tag{}, register_to_memory{}); break;
  case 0x33: inc(SP_register_tag{}); break;
  case 0x34: inc(*HL); break;
  case 0x35: dec(*HL); break;
  case 0x36: ld(*HL, n8{fetchByte()}); break;
  case 0x37: scf(); break;
  case 0x38: jr(cc::c, e8{static_cast<int8_t>(fetchByte())}); break;
  case 0x39: add(HL_register_tag{}, SP_register_tag{}); break;
  case 0x3a: ld(memory_to_register{}, HLd_tag{}); break;
  case 0x3b: dec(SP_register_tag{}); break;
  case 0x3c: inc(A); break;
  case 0x3d: dec(A); break;
  case 0x3e: ld(A, n8{fetchByte()}); break;
  case 0x3f: ccf(); break;

  case 0x40: ld(B, B); break;
  case 0x41: ld(B, C); break;
  case 0x42: ld(B, D); break;
  case 0x43: ld(B, E); break;
  case 0x44: ld(B, H); break;
  case 0x45: ld(B, L); break;
  case 0x46: ld(B, *HL); break;
  case 0x47: ld(B, A); break;

  case 0x48: ld(C, B); break;
  case 0x49: ld(C, C); break;
  case 0x4a: ld(C, D); break;
  case 0x4b: ld(C, E); break;
  case 0x4c: ld(C, H); break;
  case 0x4d: ld(C, L); break;
  case 0x4e: ld(C, *HL); break;
  case 0x4f: ld(C, A); break;

  case 0x50: ld(D, B); break;
  case 0x51: ld(D, C); break;
  case 0x52: ld(D, D); break;
  case 0x53: ld(D, E); break;
  case 0x54: ld(D, H); break;
  case 0x55: ld(D, L); break;
  case 0x56: ld(D, *HL); break;
  case 0x57: ld(D, A); break;

  case 0x58: ld(E, B); break;
  case 0x59: ld(E, C); break;
  case 0x5a: ld(E, D); break;
  case 0x5b: ld(E, E); break;
  case 0x5c: ld(E, H); break;
  case 0x5d: ld(E, L); break;
  case 0x5e: ld(E, *HL); break;
  case 0x5f: ld(E, A); break;

  case 0x60: ld(H, B); break;
  case 0x61: ld(H, C); break;
  case 0x62: ld(H, D); break;
  case 0x63: ld(H, E); break;
  case 0x64: ld(H, H); break;
  case 0x65: ld(H, L); break;
  case 0x66: ld(H, *HL); break;
  case 0x67: ld(H, A); break;
  case 0x68: ld(L, B); break;
  case 0x69: ld(L, C); break;
  case 0x6a: ld(L, D); break;
  case 0x6b: ld(L, E); break;
  case 0x6c: ld(L, H); break;
  case 0x6d: ld(L, L); break;
  case 0x6e: ld(L, *HL); break;
  case 0x6f: ld(L, A); break;
  case 0x70: ld(*HL, B); break;
  case 0x71: ld(*HL, C); break;
  case 0x72: ld(*HL, D); break;
  case 0x73: ld(*HL, E); break;
  case 0x74: ld(*HL, H); break;
  case 0x75: ld(*HL, L); break;
  case 0x76: halt(); break;
  case 0x77: ld(*HL, register_to_memory{}); break;
  case 0x78: ld(A, B); break;
  case 0x79: ld(A, C); break;
  case 0x7a: ld(A, D); break;
  case 0x7b: ld(A, E); break;
  case 0x7c: ld(A, H); break;
  case 0x7d: ld(A, L); break;
  case 0x7e: ld(A, *HL); break;
  case 0x7f: ld(A, A); break;

  case 0x80: add(B); break;
  case 0x81: add(C); break;
  case 0x82: add(D); break;
  case 0x83: add(E); break;
  case 0x84: add(H); break;
  case 0x85: add(L); break;
  case 0x86: add(*HL); break;
  case 0x87: add(A); break;

  case 0x88: adc(B); break;
  case 0x89: adc(C); break;
  case 0x8a: adc(D); break;
  case 0x8b: adc(E); break;
  case 0x8c: adc(H); break;
  case 0x8d: adc(L); break;
  case 0x8e: adc(*HL); break;
  case 0x8f: adc(A); break;

  case 0x90: sub(B); break;
  case 0x91: sub(C); break;
  case 0x92: sub(D); break;
  case 0x93: sub(E); break;
  case 0x94: sub(H); break;
  case 0x95: sub(L); break;
  case 0x96: sub(*HL); break;
  case 0x97: sub(A); break;

  case 0x98: sbc(B); break;
  case 0x99: sbc(C); break;
  case 0x9a: sbc(D); break;
  case 0x9b: sbc(E); break;
  case 0x9c: sbc(H); break;
  case 0x9d: sbc(L); break;
  case 0x9e: sbc(*HL); break;
  case 0x9f: sbc(A); break;

  case 0xa0: and_(B); break;
  case 0xa1: and_(C); break;
  case 0xa2: and_(D); break;
  case 0xa3: and_(E); break;
  case 0xa4: and_(H); break;
  case 0xa5: and_(L); break;
  case 0xa6: and_(*HL); break;
  case 0xa7: and_(A); break;

  case 0xa8: xor_(B); break;
  case 0xa9: xor_(C); break;
  case 0xaa: xor_(D); break;
  case 0xab: xor_(E); break;
  case 0xac: xor_(H); break;
  case 0xad: xor_(L); break;
  case 0xae: xor_(*HL); break;
  case 0xaf: xor_(A); break;

  case 0xb0: or_(B); break;
  case 0xb1: or_(C); break;
  case 0xb2: or_(D); break;
  case 0xb3: or_(E); break;
  case 0xb4: or_(H); break;
  case 0xb5: or_(L); break;
  case 0xb6: or_(*HL); break;
  case 0xb7: or_(A); break;

  case 0xb8: cp(B); break;
  case 0xb9: cp(C); break;
  case 0xba: cp(D); break;
  case 0xbb: cp(E); break;
  case 0xbc: cp(H); break;
  case 0xbd: cp(L); break;
  case 0xbe: cp(*HL); break;
  case 0xbf: cp(A); break;
  case 0xc0: ret(cc::nz); break;
  case 0xc1: pop(BC); break;
  case 0xc2: jp(cc::nz, n16{fetchWord()}); break;
  case 0xc3: jp(n16{fetchWord()}); break;
  case 0xc4: call(cc::nz, n16{fetchWord()}); break;
  case 0xc5: push(BC); break;
  case 0xc6: add(n8{fetchByte()}); break;
  case 0xc7: rst(0x00); break;
  case 0xc8: ret(cc::z); break;
  case 0xc9: ret(); break;
  case 0xca: jp(cc::z, n16{fetchWord()}); break;
  case 0xcb:
    switch(fetchOpcode()) {
    case 0x0: rlc(B); break;
    case 0x1: rlc(C); break;
    case 0x2: rlc(D); break;
    case 0x3: rlc(E); break;
    case 0x4: rlc(H); break;
    case 0x5: rlc(L); break;
    case 0x6: rlc(*HL); break;
    case 0x7: rlc(A); break;

    case 0x8: rrc(B); break;
    case 0x9: rrc(C); break;
    case 0xa: rrc(D); break;
    case 0xb: rrc(E); break;
    case 0xc: rrc(H); break;
    case 0xd: rrc(L); break;
    case 0xe: rrc(*HL); break;
    case 0xf: rrc(A); break;

    case 0x10: rl(B); break;
    case 0x11: rl(C); break;
    case 0x12: rl(D); break;
    case 0x13: rl(E); break;
    case 0x14: rl(H); break;
    case 0x15: rl(L); break;
    case 0x16: rl(*HL); break;
    case 0x17: rl(A); break;

    case 0x18: rr(B); break;
    case 0x19: rr(C); break;
    case 0x1a: rr(D); break;
    case 0x1b: rr(E); break;
    case 0x1c: rr(H); break;
    case 0x1d: rr(L); break;
    case 0x1e: rr(*HL); break;
    case 0x1f: rr(A); break;

    case 0x20: sla(B); break;
    case 0x21: sla(C); break;
    case 0x22: sla(D); break;
    case 0x23: sla(E); break;
    case 0x24: sla(H); break;
    case 0x25: sla(L); break;
    case 0x26: sla(*HL); break;
    case 0x27: sla(A); break;

    case 0x28: sra(B); break;
    case 0x29: sra(C); break;
    case 0x2a: sra(D); break;
    case 0x2b: sra(E); break;
    case 0x2c: sra(H); break;
    case 0x2d: sra(L); break;
    case 0x2e: sra(*HL); break;
    case 0x2f: sra(A); break;

    case 0x30: swap(B); break;
    case 0x31: swap(C); break;
    case 0x32: swap(D); break;
    case 0x33: swap(E); break;
    case 0x34: swap(H); break;
    case 0x35: swap(L); break;
    case 0x36: swap(*HL); break;
    case 0x37: swap(A); break;

    case 0x38: srl(B); break;
    case 0x39: srl(C); break;
    case 0x3a: srl(D); break;
    case 0x3b: srl(E); break;
    case 0x3c: srl(H); break;
    case 0x3d: srl(L); break;
    case 0x3e: srl(*HL); break;
    case 0x3f: srl(A); break;

    case 0x40: bit(u3{0}, B); break;
    case 0x41: bit(u3{0}, C); break;
    case 0x42: bit(u3{0}, D); break;
    case 0x43: bit(u3{0}, E); break;
    case 0x44: bit(u3{0}, H); break;
    case 0x45: bit(u3{0}, L); break;
    case 0x46: bit(u3{0}, *HL); break;
    case 0x47: bit(u3{0}, A); break;
    case 0x48: bit(u3{1}, B); break;
    case 0x49: bit(u3{1}, C); break;
    case 0x4a: bit(u3{1}, D); break;
    case 0x4b: bit(u3{1}, E); break;
    case 0x4c: bit(u3{1}, H); break;
    case 0x4d: bit(u3{1}, L); break;
    case 0x4e: bit(u3{1}, *HL); break;
    case 0x4f: bit(u3{1}, A); break;
    case 0x50: bit(u3{2}, B); break;
    case 0x51: bit(u3{2}, C); break;
    case 0x52: bit(u3{2}, D); break;
    case 0x53: bit(u3{2}, E); break;
    case 0x54: bit(u3{2}, H); break;
    case 0x55: bit(u3{2}, L); break;
    case 0x56: bit(u3{2}, *HL); break;
    case 0x57: bit(u3{2}, A); break;
    case 0x58: bit(u3{3}, B); break;
    case 0x59: bit(u3{3}, C); break;
    case 0x5a: bit(u3{3}, D); break;
    case 0x5b: bit(u3{3}, E); break;
    case 0x5c: bit(u3{3}, H); break;
    case 0x5d: bit(u3{3}, L); break;
    case 0x5e: bit(u3{3}, *HL); break;
    case 0x5f: bit(u3{3}, A); break;
    case 0x60: bit(u3{4}, B); break;
    case 0x61: bit(u3{4}, C); break;
    case 0x62: bit(u3{4}, D); break;
    case 0x63: bit(u3{4}, E); break;
    case 0x64: bit(u3{4}, H); break;
    case 0x65: bit(u3{4}, L); break;
    case 0x66: bit(u3{4}, *HL); break;
    case 0x67: bit(u3{4}, A); break;
    case 0x68: bit(u3{5}, B); break;
    case 0x69: bit(u3{5}, C); break;
    case 0x6a: bit(u3{5}, D); break;
    case 0x6b: bit(u3{5}, E); break;
    case 0x6c: bit(u3{5}, H); break;
    case 0x6d: bit(u3{5}, L); break;
    case 0x6e: bit(u3{5}, *HL); break;
    case 0x6f: bit(u3{5}, A); break;
    case 0x70: bit(u3{6}, B); break;
    case 0x71: bit(u3{6}, C); break;
    case 0x72: bit(u3{6}, D); break;
    case 0x73: bit(u3{6}, E); break;
    case 0x74: bit(u3{6}, H); break;
    case 0x75: bit(u3{6}, L); break;
    case 0x76: bit(u3{6}, *HL); break;
    case 0x77: bit(u3{6}, A); break;
    case 0x78: bit(u3{7}, B); break;
    case 0x79: bit(u3{7}, C); break;
    case 0x7a: bit(u3{7}, D); break;
    case 0x7b: bit(u3{7}, E); break;
    case 0x7c: bit(u3{7}, H); break;
    case 0x7d: bit(u3{7}, L); break;
    case 0x7e: bit(u3{7}, *HL); break;
    case 0x7f: bit(u3{7}, A); break;

    case 0x80: res(u3{0}, B); break;
    case 0x81: res(u3{0}, C); break;
    case 0x82: res(u3{0}, D); break;
    case 0x83: res(u3{0}, E); break;
    case 0x84: res(u3{0}, H); break;
    case 0x85: res(u3{0}, L); break;
    case 0x86: res(u3{0}, *HL); break;
    case 0x87: res(u3{0}, A); break;
    case 0x88: res(u3{1}, B); break;
    case 0x89: res(u3{1}, C); break;
    case 0x8a: res(u3{1}, D); break;
    case 0x8b: res(u3{1}, E); break;
    case 0x8c: res(u3{1}, H); break;
    case 0x8d: res(u3{1}, L); break;
    case 0x8e: res(u3{1}, *HL); break;
    case 0x8f: res(u3{1}, A); break;
    case 0x90: res(u3{2}, B); break;
    case 0x91: res(u3{2}, C); break;
    case 0x92: res(u3{2}, D); break;
    case 0x93: res(u3{2}, E); break;
    case 0x94: res(u3{2}, H); break;
    case 0x95: res(u3{2}, L); break;
    case 0x96: res(u3{2}, *HL); break;
    case 0x97: res(u3{2}, A); break;
    case 0x98: res(u3{3}, B); break;
    case 0x99: res(u3{3}, C); break;
    case 0x9a: res(u3{3}, D); break;
    case 0x9b: res(u3{3}, E); break;
    case 0x9c: res(u3{3}, H); break;
    case 0x9d: res(u3{3}, L); break;
    case 0x9e: res(u3{3}, *HL); break;
    case 0x9f: res(u3{3}, A); break;
    case 0xa0: res(u3{4}, B); break;
    case 0xa1: res(u3{4}, C); break;
    case 0xa2: res(u3{4}, D); break;
    case 0xa3: res(u3{4}, E); break;
    case 0xa4: res(u3{4}, H); break;
    case 0xa5: res(u3{4}, L); break;
    case 0xa6: res(u3{4}, *HL); break;
    case 0xa7: res(u3{4}, A); break;
    case 0xa8: res(u3{5}, B); break;
    case 0xa9: res(u3{5}, C); break;
    case 0xaa: res(u3{5}, D); break;
    case 0xab: res(u3{5}, E); break;
    case 0xac: res(u3{5}, H); break;
    case 0xad: res(u3{5}, L); break;
    case 0xae: res(u3{5}, *HL); break;
    case 0xaf: res(u3{5}, A); break;
    case 0xb0: res(u3{6}, B); break;
    case 0xb1: res(u3{6}, C); break;
    case 0xb2: res(u3{6}, D); break;
    case 0xb3: res(u3{6}, E); break;
    case 0xb4: res(u3{6}, H); break;
    case 0xb5: res(u3{6}, L); break;
    case 0xb6: res(u3{6}, *HL); break;
    case 0xb7: res(u3{6}, A); break;
    case 0xb8: res(u3{7}, B); break;
    case 0xb9: res(u3{7}, C); break;
    case 0xba: res(u3{7}, D); break;
    case 0xbb: res(u3{7}, E); break;
    case 0xbc: res(u3{7}, H); break;
    case 0xbd: res(u3{7}, L); break;
    case 0xbe: res(u3{7}, *HL); break;
    case 0xbf: res(u3{7}, A); break;

    case 0xc0: set(u3{0}, B); break;
    case 0xc1: set(u3{0}, C); break;
    case 0xc2: set(u3{0}, D); break;
    case 0xc3: set(u3{0}, E); break;
    case 0xc4: set(u3{0}, H); break;
    case 0xc5: set(u3{0}, L); break;
    case 0xc6: set(u3{0}, *HL); break;
    case 0xc7: set(u3{0}, A); break;
    case 0xc8: set(u3{1}, B); break;
    case 0xc9: set(u3{1}, C); break;
    case 0xca: set(u3{1}, D); break;
    case 0xcb: set(u3{1}, E); break;
    case 0xcc: set(u3{1}, H); break;
    case 0xcd: set(u3{1}, L); break;
    case 0xce: set(u3{1}, *HL); break;
    case 0xcf: set(u3{1}, A); break;
    case 0xd0: set(u3{2}, B); break;
    case 0xd1: set(u3{2}, C); break;
    case 0xd2: set(u3{2}, D); break;
    case 0xd3: set(u3{2}, E); break;
    case 0xd4: set(u3{2}, H); break;
    case 0xd5: set(u3{2}, L); break;
    case 0xd6: set(u3{2}, *HL); break;
    case 0xd7: set(u3{2}, A); break;
    case 0xd8: set(u3{3}, B); break;
    case 0xd9: set(u3{3}, C); break;
    case 0xda: set(u3{3}, D); break;
    case 0xdb: set(u3{3}, E); break;
    case 0xdc: set(u3{3}, H); break;
    case 0xdd: set(u3{3}, L); break;
    case 0xde: set(u3{3}, *HL); break;
    case 0xdf: set(u3{3}, A); break;
    case 0xe0: set(u3{4}, B); break;
    case 0xe1: set(u3{4}, C); break;
    case 0xe2: set(u3{4}, D); break;
    case 0xe3: set(u3{4}, E); break;
    case 0xe4: set(u3{4}, H); break;
    case 0xe5: set(u3{4}, L); break;
    case 0xe6: set(u3{4}, *HL); break;
    case 0xe7: set(u3{4}, A); break;
    case 0xe8: set(u3{5}, B); break;
    case 0xe9: set(u3{5}, C); break;
    case 0xea: set(u3{5}, D); break;
    case 0xeb: set(u3{5}, E); break;
    case 0xec: set(u3{5}, H); break;
    case 0xed: set(u3{5}, L); break;
    case 0xee: set(u3{5}, *HL); break;
    case 0xef: set(u3{5}, A); break;
    case 0xf0: set(u3{6}, B); break;
    case 0xf1: set(u3{6}, C); break;
    case 0xf2: set(u3{6}, D); break;
    case 0xf3: set(u3{6}, E); break;
    case 0xf4: set(u3{6}, H); break;
    case 0xf5: set(u3{6}, L); break;
    case 0xf6: set(u3{6}, *HL); break;
    case 0xf7: set(u3{6}, A); break;
    case 0xf8: set(u3{7}, B); break;
    case 0xf9: set(u3{7}, C); break;
    case 0xfa: set(u3{7}, D); break;
    case 0xfb: set(u3{7}, E); break;
    case 0xfc: set(u3{7}, H); break;
    case 0xfd: set(u3{7}, L); break;
    case 0xfe: set(u3{7}, *HL); break;
    case 0xff: set(u3{7}, A); break;
    }
    break;

  case 0xcc: call(cc::z, n16{fetchWord()}); break;
  case 0xcd: call(n16{fetchWord()}); break;
  case 0xce: adc(n8{fetchByte()}); break;
  case 0xcf: rst(0x08); break;
  case 0xd0: ret(cc::nc); break;
  case 0xd1: pop(DE); break;
  case 0xd2: jp(cc::nc, n16{fetchWord()}); break;
  case 0xd3: /* unused */ break;
  case 0xd4: call(cc::nc, n16{fetchWord()}); break;
  case 0xd5: push(DE); break;
  case 0xd6: sub(n8{fetchByte()}); break;
  case 0xd7: rst(0x10); break;
  case 0xd8: ret(cc::c); break;
  case 0xd9: reti(); break;
  case 0xda: jp(cc::c, n16{fetchWord()}); break;
  case 0xdb: /* unused */ break;
  case 0xdc: call(cc::c, n16{fetchWord()}); break;
  case 0xdd: /* unused */ break;
  case 0xde: sbc(n8{fetchByte()}); break;
  case 0xdf: rst(0x18); break;
  case 0xe0: {
    const n16 nn{static_cast<uint16_t>(0xFF00 + fetchByte())};
    byte &b = m_bus.read_write(nn.m_data);
    ldh(b, register_to_memory{});
    break;
  }
  case 0xe1: pop(HL); break;
  case 0xe2: {
    const n16 nn{static_cast<uint16_t>(0xFF00 + C.data())};
    byte &b = m_bus.read_write(nn.m_data);
    ldh(b, register_to_memory{}, C_register_tag{});
    break;
  }
  case 0xe3: /* unused */ break;
  case 0xe4: /* unused */ break;
  case 0xe5: push(HL); break;
  case 0xe6: and_(n8{fetchByte()}); break;
  case 0xe7: rst(0x20); break;
  case 0xe8: add(SP_register_tag{}, e8{static_cast<int8_t>(fetchByte())}); break;
  case 0xe9: jp(HL_register_tag{}); break;
  case 0xea: {
    const n16 nn{fetchWord()};
    byte &b = m_bus.read_write(nn.m_data);
    ld(b, register_to_memory{}, tag{});
    break;
  }
  case 0xeb: /* unused */ break;
  case 0xec: /* unused */ break;
  case 0xed: /* unused */ break;
  case 0xee: xor_(n8{fetchByte()}); break;
  case 0xef: rst(0x28); break;
  case 0xf0: {
    const n16 nn{static_cast<uint16_t>(0xFF00 + fetchByte())};
    const byte b = m_bus.read(nn.m_data);
    ldh(memory_to_register{}, b);
    break;
  }
  case 0xf1: pop(AF_register_tag{}); break;
  case 0xf2: {
    const n16 nn{static_cast<uint16_t>(0xFF00 + C.data())};
    const byte b = m_bus.read(nn.m_data);
    ldh(memory_to_register{}, b, C_register_tag{});
    break;
  }
  case 0xf3: di(); break;
  case 0xf4: /* unused */ break;
  case 0xf5: push(AF_register_tag{}); break;
  case 0xf6: or_(n8{fetchByte()}); break;
  case 0xf7: rst(0x30); break;
  case 0xf8: ld(HL_register_tag{}, SP_register_tag{}, e8{static_cast<int8_t>(fetchByte())}); break;
  case 0xf9: ld(SP_register_tag{}, HL_register_tag{}); break;
  case 0xfa: {
    const n16 nn{fetchWord()};
    const byte b = m_bus.read(nn.m_data);
    ld(memory_to_register{}, b, tag{});
    break;
  }
  case 0xfb: ei(); break;
  case 0xfc: /* unused */ break;
  case 0xfd: /* unused */ break;
  case 0xfe: cp(n8{fetchByte()}); break;
  case 0xff: rst(0x38); break;
  }
}

////////////////////////////
///  Z: zero flag        ///
///  N: negation flag    ///
///  H: half carry flag  ///
///  C: carry flag       ///
///  0bZNHC'0000         ///
////////////////////////////

// 8-bit Arithmetic and Logic Instructions
void Core::adc(r8 r) noexcept {};         // adc A,r8 // // "Z" "0" "H" "C"
void Core::adc(const byte b) noexcept {}; // adc A,[HL] // "Z" "0" "H" "C"
void Core::adc(const n8 n) noexcept {};   // adc A,n8

void Core::add(const r8 r) noexcept {};   // add A,r8 // "Z" "0" "H" "C"
void Core::add(const byte b) noexcept {}; // add A,[HL] // "Z" "0" "H" "C"
void Core::add(const n8 n) noexcept {};   // add A,n8

void Core::and_(const r8 r) noexcept {};   // and A,r8 // "Z" "0" "1" "0"
void Core::and_(const byte b) noexcept {}; // and A,[HL]
void Core::and_(const n8 n) noexcept {};   // and A,n8

void Core::cp(const r8 r) noexcept {};   // cp A,r8 // "Z" "1" "H" "C"
void Core::cp(const byte b) noexcept {}; // cp A,[HL]
void Core::cp(const n8 n) noexcept {};   // cp A,n8

void Core::dec(r8 &r) noexcept {};   // dec r8 //    // "Z" "1" "H" "-"
void Core::dec(byte &b) noexcept {}; // dec [HL] // "Z" "1" "H" "-"

void Core::inc(r8 &r) noexcept {};   // inc r8 //  // "Z" "0" "H" "-"
void Core::inc(byte &b) noexcept {}; // inc [HL]

void Core::or_(const r8 r) noexcept {};   // or A,r8 // "Z" "0" "0" "0"
void Core::or_(const byte b) noexcept {}; // or A,[HL]
void Core::or_(const n8 n) noexcept {};   // or A,n8

void Core::sbc(const r8 r) noexcept {};   // sbc A,r8 // "Z" "1" "H" "C"
void Core::sbc(const byte b) noexcept {}; // sbc A,[HL]
void Core::sbc(const n8 n) noexcept {};   // sbc A,n8

void Core::sub(const r8 r) noexcept {};   // sub A,r8 // "Z" "1" "H" "C"
void Core::sub(const byte b) noexcept {}; // sub A,[HL]
void Core::sub(const n8 n) noexcept {};   // sub A,n8

void Core::xor_(const r8 r) noexcept {};   // xor A,r8 // "Z" "0" "0" "0"
void Core::xor_(const byte b) noexcept {}; // xor A,[HL]
void Core::xor_(const n8 n) noexcept {};   // xor A,n8

// // 16-bit Arithmetic Instructions
void Core::add(HL_register_tag, const r16 rr) noexcept {}; // add HL,r16  // "-" "0" "H" "C"
void Core::dec(r16 &rr) noexcept {};                       // dec r16
void Core::inc(r16 &rr) noexcept {};                       // inc r16

// // Bit Operations Instructions
void Core::bit(const u3, const r8 r) noexcept {};   // bit u3,r8 // "Z" "0" "1" "-"
void Core::bit(const u3, const byte b) noexcept {}; // bit u3,[HL] // "Z" "0" "1" "-"

void Core::res(const u3, r8 &r) noexcept {};   // res u3,r8
void Core::res(const u3, byte &b) noexcept {}; // res u3,[HL]

void Core::set(const u3, r8 &r) noexcept {};   // set u3,r8
void Core::set(const u3, byte &b) noexcept {}; // set u3,[HL]

void Core::swap(r8 &r) noexcept {};   // swap r8 // "Z" "0" "0" "0"
void Core::swap(byte &b) noexcept {}; // swap [HL]

// // Bit Shift Instructions
void Core::rl(r8 &r) noexcept {};   // rl r8 // "Z" "0" "0" "C"
void Core::rl(byte &b) noexcept {}; // rl [HL]
void Core::rla() noexcept {};       // rla // "0" "0" "0" "C"

void Core::rlc(r8 &r) noexcept {};   // rlc r8 // "Z" "0" "0" "C"
void Core::rlc(byte &b) noexcept {}; // rlc [HL]
void Core::rlca() noexcept {};       // rlca //  // "0" "0" "0" "C"

void Core::rr(r8 &r) noexcept {};   // rr r8 // "Z" "0" "0" "C"
void Core::rr(byte &b) noexcept {}; // rr [HL]
void Core::rra() noexcept {};       // rra // // "0" "0" "0" "C"

void Core::rrc(r8 &r) noexcept {};   // rrc r8 // "Z" "0" "0" "C"
void Core::rrc(byte &b) noexcept {}; // rrc [HL]
void Core::rrca() noexcept {};       // rrca // // "0" "0" "0" "C"

void Core::sla(r8 &r) noexcept {};   // sla r8 // "Z" "0" "0" "C"
void Core::sla(byte &b) noexcept {}; // sla [HL]

void Core::sra(r8 &r) noexcept {};   // sra r8 // "Z" "0" "0" "C"
void Core::sra(byte &b) noexcept {}; // sra [HL]

void Core::srl(r8 &r) noexcept {};   // srl r8 // "Z" "0" "0" "C"
void Core::srl(byte &b) noexcept {}; // srl [HL]

// Load Instructions
void Core::ld(r8 &to, const r8 from) noexcept {}; // ld r8,r8
void Core::ld(r8 &r, const n8 n) noexcept {};     // ld r8,n8
void Core::ld(r16 &rr, const n16 nn) noexcept {}; // ld r16,n16
void Core::ld(byte &b, const r8 r) noexcept {};   // ld [HL],r8
void Core::ld(byte &b, const n8 n) noexcept {};   // ld [HL],n8
void Core::ld(r8 &r, const byte b) noexcept {};   // ld r8,[HL]
//
void Core::ld(byte &b, register_to_memory) noexcept {};      // ld [r16],A
void Core::ld(byte &b, register_to_memory, tag) noexcept {}; // ld [n16],A
//
void Core::ld(memory_to_register, const byte b) noexcept {};      // ld A,[r16]
void Core::ld(memory_to_register, const byte b, tag) noexcept {}; // ld A,[n16]
//
void Core::ldh(byte &b, register_to_memory) noexcept {};                 // ldh [n16],A
void Core::ldh(byte &b, register_to_memory, C_register_tag) noexcept {}; // ldh [C],A
//
void Core::ldh(memory_to_register, const byte b) noexcept {};                 // ldh A,[n16]
void Core::ldh(memory_to_register, const byte b, C_register_tag) noexcept {}; // ldh A,[C]
//
void Core::ld(HLi_tag, register_to_memory) noexcept {}; // ld [HLI],A
void Core::ld(HLd_tag, register_to_memory) noexcept {}; // ld [HLD],A

void Core::ld(memory_to_register, HLi_tag) noexcept {}; // ld A,[HLI]
void Core::ld(memory_to_register, HLd_tag) noexcept {}; // ld A,[HLD]

// // Jumps and Subroutines
void Core::call(const n16 nn) noexcept {};              // call n16
void Core::call(const cc cc, const n16 nn) noexcept {}; // call cc,n16
void Core::jp(HL_register_tag) noexcept {};             // jp HL
void Core::jp(const n16 nn) noexcept {};                // jp n16
void Core::jp(const cc cc, const n16 nn) noexcept {};   // jp cc,n16
void Core::jr(const e8 e) noexcept {};                  // jr e8
void Core::jr(const cc cc, const e8 e) noexcept {};     // jr cc,e8
void Core::ret(const cc cc) noexcept {};                // ret cc
void Core::ret() noexcept {};                           // ret
void Core::reti() noexcept {};                          // reti
void Core::rst(const std::size_t v) noexcept {};        // rst vec

// // Stack Operations Instructions
void Core::add(HL_register_tag, SP_register_tag) noexcept {}; // add HL,SP // "-" "0" "H" "C"
void Core::add(SP_register_tag, const e8 e) noexcept {};      // add SP,e8 // "0" "0" "H" "C"
void Core::dec(SP_register_tag) noexcept {};                  // dec SP
void Core::inc(SP_register_tag) noexcept {};                  // inc SP

void Core::ld(SP_register_tag, const n16 nn) noexcept {};                // ld SP,n16
void Core::ld(byte &b, SP_register_tag) noexcept {};                     // ld [n16],SP
void Core::ld(HL_register_tag, SP_register_tag, const e8 e) noexcept {}; // ld HL,SP+e8 // "0" "0" "H" "C"
void Core::ld(SP_register_tag, HL_register_tag) noexcept {};             // ld SP,HL

void Core::pop(AF_register_tag) noexcept {};  // pop AF // "Z" "N" "H" "C"
void Core::pop(r16 &rr) noexcept {};          // pop r16
void Core::push(AF_register_tag) noexcept {}; // push AF
void Core::push(const r16 rr) noexcept {};    // push r16

// // Miscellaneous Instructions
void Core::ccf() noexcept {};  // ccf // "-" "0" "0" "C"
void Core::cpl() noexcept {};  // cpl // // "-" "1" "1" "-"
void Core::daa() noexcept {};  // daa // // "Z" "-" "0" "C"
void Core::di() noexcept {};   // di
void Core::ei() noexcept {};   // ei
void Core::halt() noexcept {}; // halt
void Core::nop() noexcept {};  // nop
void Core::scf() noexcept {};  // scf // // "-" "0" "0" "1"
void Core::stop() noexcept {}; // stop

} // namespace LR35902