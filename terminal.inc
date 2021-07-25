    .ifndef seen_terminal
seen_terminal .equ 1

    .include "hd44780.inc"

    ZPB term_curs_col
    ZPB term_curs_row

TERM_ROWS = 2
TERM_COLS = 16

term_init:
    stz term_curs_col
    stz term_curs_row
    jsr hd44780_init
    lda #0
    jmp hd44780_set_row

; Ascii char to write is in A register.
term_write_char:
    cmp #'\n'
    beq .eol

    jsr hd44780_write_char

    ; See if we're at to the end of the line.
    lda term_curs_col
    cmp #( TERM_COLS - 1 )
    beq .eol

    ; Not at end of line yet. Increment the column and we're done.
    ina
    sta term_curs_col
    rts

.eol
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
