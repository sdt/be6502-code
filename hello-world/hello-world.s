    .include "6552.inc"
    .include "hd44780.inc"

    .org $8000

msg0    .string     "What is this?"
msg1    .string     "Could it be?"
msg2    .string     "6502 breadboard!"
msg3    .string     "Eight bit action"

; Code
reset:
    hd44780_init

    hd44780_write_register $38 ; 8-bit interface, 2-line display, 5x8 fnot
    hd44780_write_register $01 ; clear
    hd44780_write_register $02 ; home
    hd44780_write_register $0e ; display on, cursor on, blink off

loop:
    hd44780_write_string msg0, 0
    hd44780_write_string msg1, 1
    hd44780_write_register $01 ; clear
    hd44780_write_string msg2, 0
    hd44780_write_string msg3, 1
    hd44780_write_register $01 ; clear
    jmp loop

    ; Interrupt vector table.
    .include "vectors.inc"
