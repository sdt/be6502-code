    .ifndef seen_hd44780
seen_hd44780 .equ 1

    .include "6522.inc"

; Macros and constants for the hd44780 lcd character display

HD44780_CTL_PIN_RS = %00000001
HD44780_CTL_PIN_RW = %00000010
HD44780_CTL_PIN_E  = %00000100
HD44780_CTL_MASK   = HD44780_CTL_PIN_E | HD44780_CTL_PIN_RW | HD44780_CTL_PIN_RS
HD44780_CTL_DDR    = W6522_REG_DDRB
HD44780_CTL_OR     = W6522_REG_ORB
HD44780_CTL_IR     = W6522_REG_IRB

HD44780_READ       = HD44780_CTL_PIN_RW
HD44780_WRITE      = 0
HD44780_REG_DATA   = HD44780_CTL_PIN_RS
HD44780_REG_CTL    = 0

HD44780_BUSY_FLAG  = %10000000

HD44780_DATA_DDR   = W6522_REG_DDRA
HD44780_DATA_OR    = W6522_REG_ORA
HD44780_DATA_IR    = W6522_REG_IRA

HD44780_CGRAM_ADDR = %01000000  ; Or'ed with 6-bit cgram address
HD44780_DDRAM_ADDR = %10000000  ; Or'ed with 7-bit ddram address

    ZPW     hd44780_string

HD44780_INIT .macro
    ; Set control pins to be output
    lda #HD44780_CTL_MASK
    sta HD44780_CTL_DDR
    HD44780_DISABLE
    ; Set data pins to be input
    HD44780_SET_DATA_READ
    .endm

HD44780_SET_DATA_WRITE .macro
    lda #$ff
    sta HD44780_DATA_DDR
    .endm

HD44780_SET_DATA_READ .macro
    lda #$00
    sta HD44780_DATA_DDR
    .endm

HD44780_WRITE_DATA .macro value
    jsr hd44780_wait
    ; Set hd44780 to write to registers
    HD44780_SET_REGISTERS HD44780_WRITE, HD44780_REG_DATA
    ; Write the value onto the PB/data pins
    lda #\value
    jsr hd44780_write_value
    .endm

HD44780_SET_REGISTERS .macro rw, rs
value\@ = \rw | \rs
mask\@ = HD44780_CTL_PIN_RW | HD44780_CTL_PIN_RS
    WRITE_BITS HD44780_CTL_OR, value\@, mask\@
    .endm

HD44780_ENABLE .macro
    SET_BITS HD44780_CTL_OR, HD44780_CTL_PIN_E
    .endm

HD44780_DISABLE .macro
    CLEAR_BITS HD44780_CTL_OR, HD44780_CTL_PIN_E
    .endm

; WRITE_BITS address, value, mask
; => address = (address & ~mask) | value
WRITE_BITS .macro address, value, mask
invmask\@ = \mask ^ $ff
    lda \address
    and #invmask\@
    ora #\value
    sta \address
    .endm

; SET_BITS address, bits
; => address |= bits
SET_BITS .macro address, bits
    lda \address
    ora #\bits
    sta \address
    .endm

; CLEAR_BITS address, bits
; => address &= ~bits
CLEAR_BITS .macro address, bits
    lda \address
invbits\@ = \bits ^ $ff
    and #invbits\@
    sta \address
    .endm

SET_THEN_CLEAR_BITS .macro address, bits
    lda \address
    ora #\bits
invbits\@ = \bits ^ $ff
    sta \address
    and #invbits\@
    sta \address
    .endm

hd44780_wait:
    ; Set the 6522 PB pins into read mode
    HD44780_SET_DATA_READ
    ; Set hd44780 to read from registers
    HD44780_SET_REGISTERS HD44780_READ, HD44780_REG_CTL
    HD44780_ENABLE
.loop:
    bit HD44780_DATA_IR         ; sets N flag = bit 7 = busy flag
    bmi .loop                   ; if N flag is set = busy = keep looping
    HD44780_DISABLE
    rts

; Value to write is in A. A is not preserved.
; Preconditions:
;   - busy flag is clear
;   - data/inst pin is set
hd44780_write_value:
    sta HD44780_DATA_OR
    ; Set the 6522 PB pins into write mode
    HD44780_SET_DATA_WRITE
    ; Latch the value into the hd44780 by strobing the enable pin
    HD44780_ENABLE
    HD44780_DISABLE
    ; Set the 6522 PB pins back into read mode
    HD44780_SET_DATA_READ
    rts

HD44780_SET_CURSOR .macro row, col
dram_cmd\@ = HD44780_DATA_DDR | ($40 * \row) | \col
    HD44780_WRITE_REGISTER dram_cmd\@
    .endm

HD44780_WRITE_STRING_AT .macro address, row, col
    HD44780_SET_CURSOR \row, \col
    HD44780_WRITE_STRING \address
    .endm

HD44780_WRITE_STRING .macro address
    lda #<\address
    sta hd44780_string
    lda #>\address
    sta hd44780_string+1
    jsr hd44780_write_string
    .endm

hd44780_write_string:
    ldy #0
.loop:
    lda (hd44780_string),y
    beq .done      ; finished if we read a null

    jsr hd44780_wait
    ; Set hd44780 to write to registers
    HD44780_SET_REGISTERS HD44780_WRITE, HD44780_REG_DATA
    ; Write the value onto the PB/data pins
    lda (hd44780_string),y
    iny
    sta HD44780_DATA_OR
    ; Set the 6522 PB pins into write mode
    HD44780_SET_DATA_WRITE
    ; Latch the value into the hd44780 by strobing the enable pin
    HD44780_ENABLE
    HD44780_DISABLE
    jmp .loop
.done:
    ; Set the 6522 PB pins back into read mode
    HD44780_SET_DATA_READ
    rts

HD44780_WRITE_REGISTER .macro value
    lda #\value
    jsr hd44780_write_register
    .endm

hd44780_write_register:
    pha
    jsr hd44780_wait
    ; Set hd44780 to write to registers
    HD44780_SET_REGISTERS HD44780_WRITE, HD44780_REG_CTL
    ; Write the value onto the PB/data pins
    pla
    bra hd44780_write_value

hd44780_write_char:
    pha
    jsr hd44780_wait
    ; Set hd44780 to write to registers
    HD44780_SET_REGISTERS HD44780_WRITE, HD44780_REG_DATA
    ; Write the value onto the PB/data pins
    pla
    bra hd44780_write_value

; Sets the cursor to column zero on the specified row.
hd44780_set_row:
    tax
    lda _row_cmd, x
    jmp hd44780_write_register
_row_cmd    .db HD44780_DDRAM_ADDR|$00, HD44780_DDRAM_ADDR|$40

; X register is the column, Y register is the row
hd44780_set_row_col:
    lda _row_cmd, y
    sta .row_col_tmp
    txa
    clc
    adc .row_col_tmp
    jmp hd44780_write_register
    .dsect
.row_col_tmp .db 0
    .dend

HD44780_SET_CUSTOM_CHAR .macro index, address
    lda #<\address
    sta address_param_0+0
    lda #>\address
    sta address_param_0+1
    lda #( (\index << 3) | HD44780_CGRAM_ADDR)
    jsr hd44780_write_cgram_addr
    .endm

; Pointer to 8 bytes of character data are in address_param_0
; Character number 0..7 is in A register
hd44780_write_cgram:
    asl
    asl
    asl                     ; cgram address is character number * 8
    ora #HD44780_CGRAM_ADDR ; set cgram address command
hd44780_write_cgram_addr:
    jsr hd44780_write_register
    ldy #0
.loop:
    phy
    lda (address_param_0), y
    jsr hd44780_write_char
    ply
    iny
    cpy #8
    bne .loop
    rts

HD44780_SET_CURSOR .macro row, col
dram_cmd\@ = $80 | ($40 * \row) | \col
    HD44780_WRITE_REGISTER dram_cmd\@
    .endm

    .dsect
_hd44780_row_buffer .fill 16
_hd44780_row_buffer_end .db 0
    .dend

; Row to read is in register A.
; Reads to internal _hd44780_row_buffer.
hd44780_read_row:
    jsr hd44780_set_row
    ldx #0
.loop:
    jsr hd44780_wait
    HD44780_SET_DATA_READ
    HD44780_SET_REGISTERS HD44780_READ, HD44780_REG_DATA
    HD44780_ENABLE
    lda HD44780_DATA_IR
    sta _hd44780_row_buffer, x
    HD44780_DISABLE
    inx
    cpx #16
    bne .loop
    rts

_hd44780_clear_row .string "                "
hd44780_scroll_up:
    lda #1
    jsr hd44780_read_row
    lda #0
    jsr hd44780_write_row
    HD44780_WRITE_STRING_AT _hd44780_clear_row, 1, 0
    rts

; Row to write is in register A.
; Writes from internal _hd44780_row_buffer.
hd44780_write_row:
    jsr hd44780_set_row
    stz _hd44780_row_buffer_end
    HD44780_WRITE_STRING _hd44780_row_buffer
    rts

hd44780_clear:
    HD44780_WRITE_REGISTER $01 ; clear
    HD44780_WRITE_REGISTER $02 ; home
    rts

hd44780_init:
    HD44780_INIT
    HD44780_WRITE_REGISTER $38 ; 8-bit interface, 2-line display, 5x8 fnot
    HD44780_WRITE_REGISTER $01 ; clear
    HD44780_WRITE_REGISTER $02 ; home
    HD44780_WRITE_REGISTER $0e ; display on, cursor on, blink off
    rts

    .endif ; seen_hd44780
