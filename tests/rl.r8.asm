section "rom", rom0[$0000]

main:
scf

; precondition F = {.z=0, .n=0, .h=0, .c=1}
ld b, %0000_0000

; C <- [7-0] <- C
; operation | expected postconditions:
rl b        ; a = %0000_0001 F = {.z=0, .n=0, .h=0, .c=0}
