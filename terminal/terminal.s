    .include "prelude.inc"
    .include "terminal.inc"

           ;0123456789abcdef
msg    .db "One two three\n"
       .db "four five six\n"
       .db "seven eight nine"
       .db "ten eleven\n"
       .db "twelve thirteen\n"
       .db "fourteen fifteen"
       .db "sixteen\n"
       .db "seventeen\n"
       .db "eighteen\n"
       .db "nineteen twenty\n"
       .db 0

start:
    jsr term_init

again:
    ldy #0
loop:
    lda msg,y
    beq again
    iny
    phy
    jsr term_write_char
    ply
    jsr delay
    bra loop

done:
    jmp done

delay:
    phx
    phy
    ldy #255
.outer:
    ldx #255
.inner:
    dex
    bne .inner
    dey
    bne .outer
    ply
    plx
    rts

    .include "vectors.inc"
