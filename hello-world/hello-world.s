    .include "prelude.inc"
    .include "6522.inc"
    .include "hd44780.inc"

                    ;0123456789abcdef
msg0    .string     "65c02 at one MHz"
msg1    .string     "We can go faster"
msg2    .string     "Eight bit action"
msg3    .string     "6502 breadboard!"

; Code
start:
    jsr hd44780_init

loop:
    ;HD44780_WRITE_REGISTER $01 ; clear
    HD44780_WRITE_STRING_AT msg0, 0, 0
    HD44780_WRITE_STRING_AT msg1, 1, 0
;    jsr delay
    ;HD44780_WRITE_REGISTER $01 ; clear
    HD44780_WRITE_STRING_AT msg2, 0, 0
    HD44780_WRITE_STRING_AT msg3, 1, 0
;    jsr delay
    jmp loop
end:
    jmp end

delay:
    jsr .delay2
.delay2:
    jsr .delay3
.delay3
    ldy #0
.outer:
    ldx #0
.inner:
    dex
    bne .inner
    dey
    bne .outer
    rts

    ; Interrupt vector table.
    .include "vectors.inc"
