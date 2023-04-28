section "rom", rom0[$0000]

main:
ld a, %01_01_01_01
; https://rgbds.gbdev.io/docs/v0.5.2/gbz80.7/#OR_A,n8

; Flags = { z, n, h, c }

; precondition: a = %01_01_01_01, F = {.z=0, .n=0, .h=0, .c=0}
; operation         | expected postconditions:
or a, %00_00_00_10   ; a = %01_01_01_11, F = {.z=0, .n=0, .h=0, .c=0}
