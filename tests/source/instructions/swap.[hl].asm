section "rom", rom0[$0000]

main:
ld [hl], %1111_0000
swap [hl]
