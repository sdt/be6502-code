    .include "prelude.inc"
    .include "terminal.inc"

               ;0123456789abcdef0123456789abcdef
msg    .string "One two three four five six seven eight nine ten eleven twelve thirteen fourteen fifteen sixteen seventeen eighteen nineteen twenty"

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
    ldy #66
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
