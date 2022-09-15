section "rom", rom0[$0000]

main:
ld a, %11_11_11_11

; https://rgbds.gbdev.io/docs/v0.5.2/gbz80.7/#RES_u3,r8

; precondition: A = %11_11_11_11
; operation | expected postconditions:
res 0, a      ; A = %11_11_11_10
res 1, a      ; A = %11_11_11_00
res 2, a      ; A = %11_11_10_00
res 3, a      ; A = %11_11_00_00

res 4, a      ; A = %11_10_00_00
res 5, a      ; A = %11_00_00_00
res 6, a      ; A = %10_00_00_00
res 7, a      ; A = %00_00_00_00
