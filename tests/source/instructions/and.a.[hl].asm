section "rom", rom0[$0000]

main:
ld [hl], %01_01_01_01
ld a, %10_10_10_10

; https://rgbds.gbdev.io/docs/v0.5.2/gbz80.7/#AND_A,_HL_

; Flags = { z, n, h, c }

; precondition: a = %10_10_10_10, F = {.z=0, .n=0, .h=0, .c=0}
; operation     | expected postconditions:
and a, [hl]     ; a = %00_00_00_00, F = {.z=1, .n=0, .h=1, .c=0}
