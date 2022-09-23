section "rom", rom0[$0000]

main:
; C -> [7 -> 0] -> C

scf
ld b, %0000_0000
rr b

