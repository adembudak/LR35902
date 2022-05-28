#pragma once

#include "immediate//e8.h"
#include "immediate/n16.h"
#include "immediate/n8.h"
#include "immediate/u3.h"
#include "registers/cc.h"
#include "registers/flags.h"
#include "registers/r16.h"
#include "registers/r8.h"

#include <array>

// note:  *() should return byte&
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

public:
  void run();

private:
  // 8-bit Arithmetic and Logic Instructions
  void adc(const r8 r);   // adc A,r8
  void adc(const byte b); // adc A,[HL]
  void adc(const n8 n);   // adc A,n8

  void add(const r8 r);   // add A,r8
  void add(const byte b); // add A,[HL]
  void add(const n8 n);   // add A,n8

  void and_(const r8 r);   // and A,r8
  void and_(const byte b); // and A,[HL]
  void and_(const n8 n);   // and A,n8

  void cp(const r8 r);   // cp A,r8
  void cp(const byte b); // cp A,[HL]
  void cp(const n8 n);   // cp A,n8

  void dec(r8 &r);   // dec r8
  void dec(byte &b); // dec [HL]

  void inc(r8 &r);   // inc r8
  void inc(byte &b); // inc [HL]

  void or_(const r8 r);   // or A,r8
  void or_(const byte b); // or A,[HL]
  void or_(const n8 n);   // or A,n8

  void sbc(const r8 r);   // sbc A,r8
  void sbc(const byte b); // sbc A,[HL]
  void sbc(const n8 n);   // sbc A,n8

  void sub(const r8 r);   // sub A,r8
  void sub(const byte b); // sub A,[HL]
  void sub(const n8 n);   // sub A,n8

  void xor_(const r8 r);   // xor A,r8
  void xor_(const byte b); // xor A,[HL]
  void xor_(const n8 n);   // xor A,n8

  // // 16-bit Arithmetic Instructions
  void add(HL_register_tag, const r16 rr); // add HL,r16
  void dec(r16 &rr);                       // dec r16
  void inc(r16 &rr);                       // inc r16

  // // Bit Operations Instructions
  void bit(const u3, const r8 r);   // bit u3,r8
  void bit(const u3, const byte b); // bit u3,[HL]

  void res(const u3, r8 &r);   // res u3,r8
  void res(const u3, byte &b); // res u3,[HL]

  void set(const u3, r8 &r);   // set u3,r8
  void set(const u3, byte &b); // set u3,[HL]

  void swap(r8 &r);   // swap r8
  void swap(byte &b); // swap [HL]

  // // Bit Shift Instructions
  void rl(r8 &r);   // rl r8
  void rl(byte &b); // rl [HL]
  void rla();       // rla

  void rlc(r8 &r);   // rlc r8
  void rlc(byte &b); // rlc [HL]
  void rlca();       // rlca

  void rr(r8 &r);   // rr r8
  void rr(byte &b); // rr [HL]
  void rra();       // rra

  void rrc(r8 &r);   // rrc r8
  void rrc(byte &b); // rrc [HL]
  void rrca();       // rrca

  void sla(r8 &r);   // sla r8
  void sla(byte &b); // sla [HL]

  void sra(r8 &r);   // sra r8
  void sra(byte &b); // sra [HL]

  void srl(r8 &r);   // srl r8
  void srl(byte &b); // srl [HL]

  // revisit Load Instructions
  void ld(r8 &to, const r8 from); // ld r8,r8
  void ld(r8 &r, const n8 n);     // ld r8,n8
  void ld(r16 &rr, const n16 nn); // ld r16,n16
  void ld(byte &b, const r8 r);   // ld [HL],r8
  void ld(byte &b, const n8 n);   // ld [HL],n8
  void ld(r8 &r, const byte b);   // ld r8,[HL]
  //
  void ld(byte &b, load_from_A_tag);      // ld [r16],A
  void ld(byte &b, load_from_A_tag, tag); // ld [n16],A
  //
  void ld(load_to_A_tag, const byte b);      // ld A,[r16]
  void ld(load_to_A_tag, const byte b, tag); // ld A,[n16]
  //
  void ldh(byte &b, load_from_A_tag);                 // ldh [n16],A
  void ldh(byte &b, load_from_A_tag, C_register_tag); // ldh [C],A
  //
  void ldh(load_to_A_tag, const byte b); // ldh A,[n16]
  // ldh A,[C]
  //
  // ld [HLI],A
  // ld [HLD],A
  //
  // ld A,[HLI]
  // ld A,[HLD]

  // // Jumps and Subroutines
  void call(const n16 nn);              // call n16
  void call(const cc cc, const n16 nn); // call cc,n16
  void jp(HL_register_tag);             // jp HL
  void jp(const n16 nn);                // jp n16
  void jp(const cc cc, const n16 nn);   // jp cc,n16
  void jr(const e8 e);                  // jr e8
  void jr(const cc cc, const e8 e);     // jr cc,e8
  void ret(const cc cc);                // ret cc
  void ret();                           // ret
  void reti();                          // reti
  void rst(const std::size_t v);        // rst vec

  // revisit // Stack Operations Instructions
  void add(HL_register_tag, SP_register_tag); // add HL,SP
  void add(SP_register_tag, const e8 e);      // add SP,e8
  void dec(SP_register_tag);                  // dec SP
  void inc(SP_register_tag);                  // inc SP

  void ld(SP_register_tag, const n16 nn); // ld SP,n16
  void ld(byte &b, SP_register_tag);      // ld [n16],SP
  // ld HL,SP+e8
  void ld(SP_register_tag, HL_register_tag); // ld SP,HL

  void pop(AF_register_tag);  // pop AF
  void pop(r16 &rr);          // pop r16
  void push(AF_register_tag); // push AF
  void push(const r16 rr);    // push r16

  // // Miscellaneous Instructions
  void ccf();  // ccf
  void cpl();  // cpl
  void daa();  // daa
  void di();   // di
  void ei();   // ei
  void halt(); // halt
  void nop();  // nop
  void scf();  // scf
  void stop(); // stop
};

} // namespace LR35902
