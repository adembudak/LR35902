section "rom", rom0[$0000]

main:
ld b, 1
ld a, $ff

rept $ff
sub a, b
endr

