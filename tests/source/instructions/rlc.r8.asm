section "rom", rom0[$0000]

main:
; C <- [7 <- 0] <- [7]

ld b, %1000_0000
rlc b

