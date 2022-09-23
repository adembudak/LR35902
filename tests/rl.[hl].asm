section "rom", rom0[$0000]

main:
scf

; precondition F = {.z=0, .n=0, .h=0, .c=1}
ld [hl], %0000_0000

; operation | expected postconditions:
rl [hl]     ; [hl] = %0000_0001 F = {.z=0, .n=0, .h=0, .c=0}

; precondition F = {.z=0, .n=0, .h=0, .c=0}
ld [hl], %1000_0000

; operation | expected postconditions:
rl [hl]     ; [hl] = %0000_0000 F = {.z=1, .n=0, .h=0, .c=1}
