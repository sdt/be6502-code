    .include "6552.inc"
    .include "hd44780.inc"

    .org $8000

; Code
reset:
    hd44780_init

    hd44780_write_register $38 ; 8-bit interface, 2-line display, 5x8 fnot
    hd44780_write_register $01 ; clear
    hd44780_write_register $02 ; home
    hd44780_write_register $0e ; display on, cursor on, blink off

    hd44780_write_register $80 ; dram address = 0 => 0,0
    hd44780_write_data '6'
    hd44780_write_data '5'
    hd44780_write_data '0'
    hd44780_write_data '2'
    hd44780_write_data ' '
    hd44780_write_data 'b'
    hd44780_write_data 'r'
    hd44780_write_data 'e'
    hd44780_write_data 'a'
    hd44780_write_data 'd'
    hd44780_write_data 'b'
    hd44780_write_data 'o'
    hd44780_write_data 'a'
    hd44780_write_data 'r'
    hd44780_write_data 'd'
    hd44780_write_data '!'

    hd44780_write_register $C0 ; dram address = $40 => 0,1
    hd44780_write_data 'E'
    hd44780_write_data 'i'
    hd44780_write_data 'g'
    hd44780_write_data 'h'
    hd44780_write_data 't'
    hd44780_write_data ' '
    hd44780_write_data 'b'
    hd44780_write_data 'i'
    hd44780_write_data 't'
    hd44780_write_data ' '
    hd44780_write_data 'a'
    hd44780_write_data 'c'
    hd44780_write_data 't'
    hd44780_write_data 'i'
    hd44780_write_data 'o'
    hd44780_write_data 'n'

loop:
    jmp loop

    ; Interrupt vector table.
irq = $efbe
    .include "vectors.inc"
