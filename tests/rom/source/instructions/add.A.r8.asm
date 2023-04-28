section "rom", rom0[$0000]

main:
ld b, $01
ld a, $0f

; precondition: F = {.z=0, .n=0, .h=0, .c=0}

; operation | expected postconditions:
add a, b  ;  a = $10, F ={.z=0, .n=0, .h=1, .c=0}

ld a, $ff
add a, b  ;  a = $00, F ={.z=1, .n=0, .h=1, .c=1}

ld b, $ff

ld a, $ff
add a, b  ;  a = $fe, F ={.z=0, .n=0, .h=1, .c=1}

