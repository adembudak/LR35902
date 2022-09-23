section "rom", rom0[$0000]

main:
; C -> [7 -> 0] -> C

scf
ld a, %0000_0000
rra

