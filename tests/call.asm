section "rom", rom0[$0000]

main:
ld sp, $fffe
call load_10
nop

load_10:
ld A, $10
ld B, $10
ld C, $10
ret

