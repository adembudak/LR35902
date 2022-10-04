section "rom", rom0[$0000]

main:
ld a, $f

rept $f
sub a, 1
endr

