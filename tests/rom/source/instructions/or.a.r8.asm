section "rom", rom0[$0000]

main:
ld b, %10_10_10_10
ld a, %01_01_01_01

; https://rgbds.gbdev.io/docs/v0.5.2/gbz80.7/#OR_A,r8

; Flags = { z, n, h, c }

; precondition: a = %01_01_01_01, F = {.z=0, .n=0, .h=0, .c=0}
; operation | expected postconditions:
or a, b     ; a = %11_11_11_11, F = {.z=0, .n=0, .h=0, .c=0}
