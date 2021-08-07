    .ifndef seen_terminal
seen_terminal .equ 1

    .include "hd44780.inc"
    .include "itoa.inc"

    ZPB term_curs_col
    ZPB term_curs_row

TERM_ROWS = 2
TERM_COLS = 16

term_init:
    stz term_curs_col
    stz term_curs_row
    jsr hd44780_init

    HD44780_SET_CUSTOM_CHAR CUSTOM_CHAR_TILDE,     .custom_char_tilde
    HD44780_SET_CUSTOM_CHAR CUSTOM_CHAR_BACKSLASH, .custom_char_backslash

    lda #0
    jmp hd44780_set_row

; Note: We have a bug in hd44780_scroll_up - we copy 16 chars into the row
; buffer, but we call hd44780_write_string to write that buffer back out. If
; there's a zero in that buffer, we stop. As a workaround, we use the fact
; that the eight custom chars in hd44780 get repeated for 0-7 and then 8-15.
; To avoid the zero problem, we use index+8 - the chars look the same, but we
; don't write any zeros.
CUSTOM_CHAR_TILDE = 0
.custom_char_tilde:
    .db %00000
    .db %00000
    .db %01101
    .db %10010
    .db %00000
    .db %00000
    .db %00000
    .db %00000

CUSTOM_CHAR_BACKSLASH = 1
.custom_char_backslash:
    .db %00000
    .db %10000
    .db %01000
    .db %00100
    .db %00010
    .db %00001
    .db %00000
    .db %00000

    ZPW term_string

TERM_WRITE_STRING .macro address
    lda #<\address
    sta term_string
    lda #>\address
    sta term_string+1
    jsr term_write_string
    .endm

term_write_string:
    ldy #0
.loop:
    lda (term_string),y
    beq .done      ; finished if we read a null
    iny
    phy
    jsr term_write_char
    ply
    jmp .loop

.done:
    rts

; Write byte in A as hex.
term_write_hex_byte:
    pha
    lsr
    lsr
    lsr
    lsr
    jsr .write_hex_nybble
    pla
    and #%00001111
.write_hex_nybble:
    tax
    lda .hex_chars, x
    jmp term_write_char

.hex_chars .db '0123456789abcdef'

TERM_WRITE_U16 .macro address
    lda \address+0
    sta itoa_in+0
    lda \address+1
    sta itoa_in+1
    jsr itoa
    ldx #0
.write_digits:
    lda itoa_out,x
    beq .done
    phx
    jsr term_write_char
    plx
    inx
    bra .write_digits
.done:
    .endm

REPLACE_CHAR .macro char, replacement
    cmp #\char
    bne .not_custom_char\@
    lda #(\replacement + 8) ; see note above about hd44780_scroll_up bug
    bra .write_char
.not_custom_char\@:
    .endm

; Ascii char to write is in A register.
term_write_char:
    cmp #'\n'
    beq .eol

    REPLACE_CHAR '~',  CUSTOM_CHAR_TILDE
    REPLACE_CHAR '\\', CUSTOM_CHAR_BACKSLASH

.write_char:
    jsr hd44780_write_char

    ; See if we're at to the end of the line.
    lda term_curs_col
    cmp #( TERM_COLS - 1 )
    beq .eol

    ; Not at end of line yet. Increment the column and we're done.
    ina
    sta term_curs_col
    rts

.eol:
    ; We've hit the end of the line. We want to wrap to the next line, but do we
    ; need to scroll?
    stz term_curs_col   ; in all cases, we're back to column zero

    lda term_curs_row
    cmp #( TERM_ROWS - 1 )
    beq .scroll

    ; Don't need to scroll. Increment the cursor row.
    ina
    sta term_curs_row
    jmp hd44780_set_row

.scroll:
    ; We need to scroll. The row stays the same, but we need to shift the dram
    ; contents one line upward. Row 1 gets copied to row 0. Row 2 gets copied to
    ; row one etc. Bottom row gets cleared.
    jsr hd44780_scroll_up
    lda #( TERM_ROWS - 1 )  ; Now reset the cursor back to the first column
    jmp hd44780_set_row     ; on the bottom row.

    .endif ; seen_terminal
