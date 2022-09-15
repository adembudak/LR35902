section "rom", rom0[$0000]

main:
ld a, %10_10_10_10
ld [hl], a

; https://rgbds.gbdev.io/docs/v0.5.2/gbz80.7/#BIT_u3,_HL_

; Flags = { z, n, h, c }

; precondition: F = {.z=0, .n=0, .h=0, .c=0}
; operation | expected postconditions:
bit 0, [hl]      ; F = {.z=1, .n=0, .h=1, .c=0}
bit 1, [hl]      ; F = {.z=0, .n=0, .h=1, .c=0}
bit 2, [hl]      ; F = {.z=1, .n=0, .h=1, .c=0}
bit 3, [hl]      ; F = {.z=0, .n=0, .h=1, .c=0}
 
bit 4, [hl]      ; F = {.z=1, .n=0, .h=1, .c=0}
bit 5, [hl]      ; F = {.z=0, .n=0, .h=1, .c=0}
bit 6, [hl]      ; F = {.z=1, .n=0, .h=1, .c=0}
bit 7, [hl]      ; F = {.z=0, .n=0, .h=1, .c=0}
