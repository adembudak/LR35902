section "rom", rom0[$0000]

main:
ld b, %10_10_10_10
ld a, %01_01_01_01

xor a, b
