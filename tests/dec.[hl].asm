section "rom", rom0[$0000]

main:
ld [hl], %0001_0000
dec [hl]

ld [hl], %0000_0001
dec [hl]
