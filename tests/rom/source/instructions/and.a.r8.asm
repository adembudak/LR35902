section "rom", rom0[$0000]

main:
ld b, %00_00_00_00
ld a, %11_11_11_11

; https://rgbds.gbdev.io/docs/v0.5.2/gbz80.7/#AND_A,r8

; Flags = { z, n, h, c }

; precondition: a = %11_11_11_11, F = {.z=0, .n=0, .h=0, .c=0}
; operation     | expected postconditions:
and a, b        ; a = %00_00_00_00, F = {.z=1, .n=0, .h=1, .c=0}
