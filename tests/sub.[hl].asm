section "rom", rom0[$0000]

main:
ld [hl], 1
ld a, $f

rept $f
sub a, [hl]
endr

