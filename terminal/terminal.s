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

msg2 .ascii "All the way\nback to ..."

    .dsect
number .dw 0
    .dend

start:
    jsr term_init
    lda #<10000
    sta number+0
    lda #>10000
    sta number+1

numloop:
    TERM_WRITE_U16 number
    lda #' '
    jsr term_write_char
    jsr delay
    inc number+0
    bne .no_carry
    inc number+1
.no_carry
    bra numloop

    lda #0
hexloop:
    pha
    jsr term_write_hex_byte
    lda #' '
    jsr term_write_char
    jsr delay
    pla
    ina
    bne hexloop

again:
    ldy #0
loop:
    lda msg,y
    beq next
    iny
    phy
    jsr term_write_char
    ply
    jsr delay
    bra loop

next:
    TERM_WRITE_STRING msg2
    jsr delay
    jsr delay
    jsr delay
    jsr delay
    jsr delay
    jsr delay
    jsr delay
    jsr delay
    lda #'\n'
    jsr term_write_char
    bra again

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
