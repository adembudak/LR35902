section "rom", rom0[$0000]

main:
; C <- [7 <- 0] <- [7]

ld a, %1000_0000
rlca

