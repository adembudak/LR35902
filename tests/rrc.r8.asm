section "rom", rom0[$0000]

main:

; [0] -> [7 -> 0] -> C
ld b, %1000_0001
rrc b
