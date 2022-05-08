#pragma once

#include "immediate/n16.h"
#include "registers/flags.h"
#include "registers/r16.h"
#include "registers/r8.h"

namespace LR35902 {

class core {
private:
  r8 A, F;
  r16 AF{A, F};

  r8 B, C;
  r16 BC{B, C};

  r8 D, E;
  r16 DE{D, E};

  r8 H, L;
  r16 HL{H, L};

  n16 SP;
  n16 PC;

public:
  void run();

  // instruction names taken from: https://rgbds.gbdev.io/docs/v0.5.2/gbz80.7

  // 8-bit Arithmetic and Logic Instructions
  // adc A,r8
  // adc A,[HL]
  // adc A,n8
  //
  // add A,r8
  // add A,[HL]
  // add A,n8
  //
  // and A,r8
  // and A,[HL]
  // and A,n8
  //
  // cp A,r8
  // cp A,[HL]
  // cp A,n8
  //
  // dec r8
  // dec [HL]
  //
  // inc r8
  // inc [HL]
  //
  // or A,r8
  // or A,[HL]
  // or A,n8
  //
  // sbc A,r8
  // sbc A,[HL]
  // sbc A,n8
  //
  // sub A,r8
  // sub A,[HL]
  // sub A,n8
  //
  // xor A,r8
  // xor A,[HL]
  // xor A,n8

  // // 16-bit Arithmetic Instructions
  // add HL,r16
  // dec r16
  // inc r16

  // // Bit Operations Instructions
  // bit u3,r8
  // bit u3,[HL]

  // res u3,r8
  // res u3,[HL]

  // set u3,r8
  // set u3,[HL]

  // swap r8
  // swap [HL]

  // // Bit Shift Instructions
  // rl r8
  // rl [HL]
  // rla
  // rlc r8
  // rlc [HL]
  // rlca
  // rr r8
  // rr [HL]
  // rra
  // rrc r8
  // rrc [HL]
  // rrcA
  // sla r8
  // sla [HL]
  // sra r8
  // sra [HL]
  // srl r8
  // srl [HL]

  // // Load Instructions
  // ld r8,r8
  // ld r8,n8
  // ld r16,n16
  // ld [HL],r8
  // ld [HL],n8
  // ld r8,[HL]
  // ld [r16],A
  // ld [n16],A
  // ldh [n16],A
  // ldh [C],A
  // ld A,[r16]
  // ld A,[n16]
  // ldh A,[n16]
  // ldh A,[C]
  // ld [HLI],A
  // ld [HLD],A
  // ld A,[HLI]
  // ld A,[HLD]

  // // Jumps and Subroutines
  // call n16
  // call cc,n16
  // jp HL
  // jp n16
  // jp cc,n16
  // jr e8
  // jr cc,e8
  // ret cc
  // ret
  // reti
  // rst vec

  // // Stack Operations Instructions
  // add HL,SP
  // add SP,e8
  // dec SP
  // inc SP
  // ld SP,n16
  // ld [n16],SP
  // ld HL,SP+e8
  // ld SP,HL
  // pop AF
  // pop r16
  // push AF
  // push r16

  // // Miscellaneous Instructions
  // ccf
  // cpl
  // daa
  // di
  // ei
  // halt
  // nop
  // scf
  // stop
};

} // namespace LR35902
