section "rom", rom0[$0000]

main:
ld a, %10_10_10_10

; https://rgbds.gbdev.io/docs/v0.5.2/gbz80.7/#BIT_u3,r8

; Flags = { z, n, h, c }

; precondition: F = {.z=0, .n=0, .h=0, .c=0}
; operation | expected postconditions:
bit 0, a      ; F = {.z=1, .n=0, .h=1, .c=0}
bit 1, a      ; F = {.z=0, .n=0, .h=1, .c=0}
bit 2, a      ; F = {.z=1, .n=0, .h=1, .c=0}
bit 3, a      ; F = {.z=0, .n=0, .h=1, .c=0}
 
bit 4, a      ; F = {.z=1, .n=0, .h=1, .c=0}
bit 5, a      ; F = {.z=0, .n=0, .h=1, .c=0}
bit 6, a      ; F = {.z=1, .n=0, .h=1, .c=0}
bit 7, a      ; F = {.z=0, .n=0, .h=1, .c=0}
