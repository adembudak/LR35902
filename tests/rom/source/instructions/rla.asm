section "rom", rom0[$0000]

main:
; precondition F = {.z=0, .n=0, .h=0, .c=0}
ld a, %1000_0000

; C <- [7 <- 0] <- C
; operation | expected postconditions:
rla         ; a = %0000_0000 F = {.z=0, .n=0, .h=0, .c=1}
