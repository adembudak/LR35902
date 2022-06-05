#pragma once

#include "immediate/e8.h"
#include "immediate/n16.h"
#include "immediate/n8.h"
#include "immediate/u3.h"
#include "registers/cc.h"
#include "registers/flags.h"
#include "registers/r16.h"
#include "registers/r8.h"

#include <array>

namespace LR35902 {

// instruction names and behaviors taken from:
// https://rgbds.gbdev.io/docs/v0.5.2/gbz80.7
class core {
private:
  r8 A;
  flags F;

  r8 B, C;
  r16 BC{B, C};

  r8 D, E;
  r16 DE{D, E};

  r8 H, L;
  r16 HL{H, L};

  n16 SP;
  n16 PC;

  cc cc_;
  std::array<byte, 8> rst_vec{0x00, 0x08, 0x10, 0x18, 0x20, 0x28, 0x30, 0x38};

  struct AF_register_tag {};
  struct SP_register_tag {};
  struct HL_register_tag {};
  struct C_register_tag {};
  struct HLi_tag {};
  struct HLd_tag {};

  struct load_from_A_tag {};
  struct load_to_A_tag {};
  struct tag {};

  auto fetchByte() noexcept -> byte;
  auto fetchWord() noexcept -> word;

public:
  void run() noexcept;

  friend class debugView;

private:
  // 8-bit Arithmetic and Logic Instructions
  void adc(const r8 r) noexcept;   // adc A,r8
  void adc(const byte b) noexcept; // adc A,[HL]
  void adc(const n8 n) noexcept;   // adc A,n8

  void add(const r8 r) noexcept;   // add A,r8
  void add(const byte b) noexcept; // add A,[HL]
  void add(const n8 n) noexcept;   // add A,n8

  void and_(const r8 r) noexcept;   // and A,r8
  void and_(const byte b) noexcept; // and A,[HL]
  void and_(const n8 n) noexcept;   // and A,n8

  void cp(const r8 r) noexcept;   // cp A,r8
  void cp(const byte b) noexcept; // cp A,[HL]
  void cp(const n8 n) noexcept;   // cp A,n8

  void dec(r8 &r) noexcept;   // dec r8
  void dec(byte &b) noexcept; // dec [HL]

  void inc(r8 &r) noexcept;   // inc r8
  void inc(byte &b) noexcept; // inc [HL]

  void or_(const r8 r) noexcept;   // or A,r8
  void or_(const byte b) noexcept; // or A,[HL]
  void or_(const n8 n) noexcept;   // or A,n8

  void sbc(const r8 r) noexcept;   // sbc A,r8
  void sbc(const byte b) noexcept; // sbc A,[HL]
  void sbc(const n8 n) noexcept;   // sbc A,n8

  void sub(const r8 r) noexcept;   // sub A,r8
  void sub(const byte b) noexcept; // sub A,[HL]
  void sub(const n8 n) noexcept;   // sub A,n8

  void xor_(const r8 r) noexcept;   // xor A,r8
  void xor_(const byte b) noexcept; // xor A,[HL]
  void xor_(const n8 n) noexcept;   // xor A,n8

  // // 16-bit Arithmetic Instructions
  void add(HL_register_tag, const r16 rr) noexcept; // add HL,r16
  void dec(r16 &rr) noexcept;                       // dec r16
  void inc(r16 &rr) noexcept;                       // inc r16

  // // Bit Operations Instructions
  void bit(const u3, const r8 r) noexcept;   // bit u3,r8
  void bit(const u3, const byte b) noexcept; // bit u3,[HL]

  void res(const u3, r8 &r) noexcept;   // res u3,r8
  void res(const u3, byte &b) noexcept; // res u3,[HL]

  void set(const u3, r8 &r) noexcept;   // set u3,r8
  void set(const u3, byte &b) noexcept; // set u3,[HL]

  void swap(r8 &r) noexcept;   // swap r8
  void swap(byte &b) noexcept; // swap [HL]

  // // Bit Shift Instructions
  void rl(r8 &r) noexcept;   // rl r8
  void rl(byte &b) noexcept; // rl [HL]
  void rla() noexcept;       // rla

  void rlc(r8 &r) noexcept;   // rlc r8
  void rlc(byte &b) noexcept; // rlc [HL]
  void rlca() noexcept;       // rlca

  void rr(r8 &r) noexcept;   // rr r8
  void rr(byte &b) noexcept; // rr [HL]
  void rra() noexcept;       // rra

  void rrc(r8 &r) noexcept;   // rrc r8
  void rrc(byte &b) noexcept; // rrc [HL]
  void rrca() noexcept;       // rrca

  void sla(r8 &r) noexcept;   // sla r8
  void sla(byte &b) noexcept; // sla [HL]

  void sra(r8 &r) noexcept;   // sra r8
  void sra(byte &b) noexcept; // sra [HL]

  void srl(r8 &r) noexcept;   // srl r8
  void srl(byte &b) noexcept; // srl [HL]

  // Load Instructions
  void ld(r8 &to, const r8 from) noexcept; // ld r8,r8
  void ld(r8 &r, const n8 n) noexcept;     // ld r8,n8
  void ld(r16 &rr, const n16 nn) noexcept; // ld r16,n16
  void ld(byte &b, const r8 r) noexcept;   // ld [HL],r8
  void ld(byte &b, const n8 n) noexcept;   // ld [HL],n8
  void ld(r8 &r, const byte b) noexcept;   // ld r8,[HL]
  //
  void ld(byte &b, load_from_A_tag) noexcept;      // ld [r16],A
  void ld(byte &b, load_from_A_tag, tag) noexcept; // ld [n16],A
  //
  void ld(load_to_A_tag, const byte b) noexcept;      // ld A,[r16]
  void ld(load_to_A_tag, const byte b, tag) noexcept; // ld A,[n16]
  //
  void ldh(byte &b, load_from_A_tag) noexcept;                 // ldh [n16],A
  void ldh(byte &b, load_from_A_tag, C_register_tag) noexcept; // ldh [C],A
  //
  void ldh(load_to_A_tag, const byte b) noexcept;                 // ldh A,[n16]
  void ldh(load_to_A_tag, const byte b, C_register_tag) noexcept; // ldh A,[C]
  //
  void ld(HLi_tag, load_from_A_tag) noexcept; // ld [HLI],A
  void ld(HLd_tag, load_from_A_tag) noexcept; // ld [HLD],A

  void ld(load_to_A_tag, HLi_tag) noexcept; // ld A,[HLI]
  void ld(load_to_A_tag, HLd_tag) noexcept; // ld A,[HLD]

  // // Jumps and Subroutines
  void call(const n16 nn) noexcept;              // call n16
  void call(const cc cc, const n16 nn) noexcept; // call cc,n16
  void jp(HL_register_tag) noexcept;             // jp HL
  void jp(const n16 nn) noexcept;                // jp n16
  void jp(const cc cc, const n16 nn) noexcept;   // jp cc,n16
  void jr(const e8 e) noexcept;                  // jr e8
  void jr(const cc cc, const e8 e) noexcept;     // jr cc,e8
  void ret(const cc cc) noexcept;                // ret cc
  void ret() noexcept;                           // ret
  void reti() noexcept;                          // reti
  void rst(const std::size_t v) noexcept;        // rst vec

  // // Stack Operations Instructions
  void add(HL_register_tag, SP_register_tag) noexcept; // add HL,SP
  void add(SP_register_tag, const e8 e) noexcept;      // add SP,e8
  void dec(SP_register_tag) noexcept;                  // dec SP
  void inc(SP_register_tag) noexcept;                  // inc SP

  void ld(SP_register_tag, const n16 nn) noexcept;                // ld SP,n16
  void ld(byte &b, SP_register_tag) noexcept;                     // ld [n16],SP
  void ld(HL_register_tag, SP_register_tag, const e8 e) noexcept; // ld HL,SP+e8
  void ld(SP_register_tag, HL_register_tag) noexcept;             // ld SP,HL

  void pop(AF_register_tag) noexcept;  // pop AF
  void pop(r16 &rr) noexcept;          // pop r16
  void push(AF_register_tag) noexcept; // push AF
  void push(const r16 rr) noexcept;    // push r16

  // // Miscellaneous Instructions
  void ccf() noexcept;  // ccf
  void cpl() noexcept;  // cpl
  void daa() noexcept;  // daa
  void di() noexcept;   // di
  void ei() noexcept;   // ei
  void halt() noexcept; // halt
  void nop() noexcept;  // nop
  void scf() noexcept;  // scf
  void stop() noexcept; // stop
};

} // namespace LR35902
