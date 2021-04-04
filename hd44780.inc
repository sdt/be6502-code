; Macros and constants for the hd44780 lcd character display
; Requires 6552.inc

HD44780_CTL_PIN_E  = $80
HD44780_CTL_PIN_RW = $40
HD44780_CTL_PIN_RS = $20
HD44780_CTL_MASK   = HD44780_CTL_PIN_E | HD44780_CTL_PIN_RW | HD44780_CTL_PIN_RS
HD44780_CTL_DDR    = W6552_REG_DDRA
HD44780_CTL_OR     = W6552_REG_ORA
HD44780_CTL_IR     = W6552_REG_IRA

HD44780_ENABLE     = HD44780_CTL_PIN_E
HD44780_DISABLE    = 0
HD44780_READ       = HD44780_CTL_PIN_RW
HD44780_WRITE      = 0
HD44780_REG_DATA   = HD44780_CTL_PIN_RS
HD44780_REG_CTL    = 0

HD44780_BUSY_FLAG  = $80

HD44780_DATA_DDR   = W6552_REG_DDRB
HD44780_DATA_OR    = W6552_REG_ORA

hd44780_init .macro
    ; Set control pins to be output
    lda #HD44780_CTL_MASK
    sta HD44780_CTL_DDR
    hd44780_disable
    ; Set data pins to be input
    hd44780_set_data_read
    .endm

hd44780_set_data_write .macro
    lda #$ff
    sta HD44780_DATA_DDR
    .endm

hd44780_set_data_read .macro
    lda #$00
    sta HD44780_DATA_DDR
    .endm

hd44780_write_register .macro value
    hd44780_wait
    ; Set the 6522 PB pins into write mode
    hd44780_set_data_write
    ; Set hd44780 to write to registers
    hd44780_set_registers HD44780_WRITE, HD44780_REG_CTL
    ; Write the value onto the PB/data pins
    lda #\value
    sta HD44780_DATA_OR
    ; Latch the value into the hd44780 by strobing the enable pin
    hd44780_enable
    hd44780_disable
    ; Set the 6522 PB pins back into read mode
    hd44780_set_data_read
    .endm

hd44780_write_data .macro value
    hd44780_wait
    ; Set the 6522 PB pins into write mode
    hd44780_set_data_write
    ; Set hd44780 to write to registers
    hd44780_set_registers HD44780_WRITE, HD44780_REG_DATA
    ; Write the value onto the PB/data pins
    lda #\value
    sta HD44780_DATA_OR
    ; Latch the value into the hd44780 by strobing the enable pin
    hd44780_enable
    hd44780_disable
    ; Set the 6522 PB pins back into read mode
    hd44780_set_data_read
    .endm

hd44780_wait .macro
    ; Set the 6522 PB pins into read mode
    hd44780_set_data_read
    ; Set hd44780 to read from registers
    hd44780_set_registers HD44780_READ, HD44780_REG_CTL
loop\@:
    hd44780_disable
    hd44780_enable
    bit HD44780_CTL_IR          ; sets N flag = bit 7 = busy flag
    bmi loop\@                  ; if N flag is set = busy = keep looping
    hd44780_disable
    .endm

hd44780_begin_write .macro
    hd44780_wait
    hd44780_set_data_write
    .endm

hd44780_set_registers .macro rw, rs
value\@ = \rw | \rs
mask\@ = HD44780_CTL_PIN_RW | HD44780_CTL_PIN_RS
    write_bits HD44780_CTL_OR, value\@, mask\@
    .endm

hd44780_enable .macro
    set_bits HD44780_CTL_OR, HD44780_CTL_PIN_E
    .endm

hd44780_disable .macro
    clear_bits HD44780_CTL_OR, HD44780_CTL_PIN_E
    .endm

; write_bits address, value, mask
; => address = (address & ~mask) | value
write_bits .macro address, value, mask
invmask\@ = \mask ^ $ff
    lda \address
    and #invmask\@
    ora #\value
    sta \address
    .endm

; set_bits address, bits
; => address |= bits
set_bits .macro address, bits
    lda \address
    ora #\bits
    sta \address
    .endm

; clear_bits address, bits
; => address &= ~bits
clear_bits .macro address, bits
    lda \address
invbits\@ = \bits ^ $ff
    and #invbits\@
    sta \address
    .endm

; vim: set filetype=asm:
