section "rom", rom0[$0000]

main:
ld [hl], %11_11_11_11

; https://rgbds.gbdev.io/docs/v0.5.2/gbz80.7/#RES_u3,_HL_

; precondition: [hl] = %11_11_11_11
; operation | expected postconditions:
res 0, [hl]   ; [hl] = %11_11_11_10
res 1, [hl]   ; [hl] = %11_11_11_00
res 2, [hl]   ; [hl] = %11_11_10_00
res 3, [hl]   ; [hl] = %11_11_00_00

res 4, [hl]   ; [hl] = %11_10_00_00
res 5, [hl]   ; [hl] = %11_00_00_00
res 6, [hl]   ; [hl] = %10_00_00_00
res 7, [hl]   ; [hl] = %00_00_00_00
