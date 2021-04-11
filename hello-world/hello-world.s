    .dsect
    .org $0000
    ; ZERO PAGE starts here
hd44780_string  .addr   0

    .org $0200
    .dend

    .org $8000

    .include "6552.inc"
    .include "hd44780.inc"
                    ;0123456789abcdef
msg0    .string     "65c02 at one MHz"
msg1    .string     "We can go faster"
msg2    .string     "Eight bit action"
msg3    .string     "6502 breadboard!"

; Code
reset:
    hd44780_init

    hd44780_write_register $38 ; 8-bit interface, 2-line display, 5x8 fnot
    hd44780_write_register $01 ; clear
    hd44780_write_register $02 ; home
    hd44780_write_register $0e ; display on, cursor on, blink off

loop:
    ;hd44780_write_register $01 ; clear
    hd44780_write_string msg0, 0
    hd44780_write_string msg1, 1
    jsr delay
    ;hd44780_write_register $01 ; clear
    hd44780_write_string msg2, 0
    hd44780_write_string msg3, 1
    jsr delay
    jmp loop
end:
    jmp end
;    hd44780_write_register $01 ; clear

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
