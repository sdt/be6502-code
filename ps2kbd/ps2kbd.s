    .include "prelude.inc"
    .include "6522.inc"
    .include "hd44780.inc"
    .include "bit-reverse.inc"
    .include "ps2kbd.inc"

    ZPB cnt

irq:
    pha
    phx

    PS2KBD_IRQ_HANDLER

    plx
    pla

    inc cnt
    rti

; 11 + 11 + 11 = 33 = 4 * 8 + 1

msg_a .string "STA: "
msg_b .string " IFR: "
msg_c .string "SCD: "
msg_d .string " CNT: "

start:
    stz cnt
    W6522_DISABLE_INTERRUPTS W6522_IER_ALL

    jsr hd44780_init
    jsr ps2kbd_init

    cli     ; enable interrupts (I=0)

loop:
    HD44780_WRITE_STRING_AT msg_a, 0, 0
    lda ps2kbd_state
    jsr write_hex_byte

    HD44780_WRITE_STRING msg_b
    lda ifr
    jsr write_hex_byte

    HD44780_WRITE_STRING_AT msg_c, 1, 0
    lda scancode
    jsr write_hex_byte

    HD44780_WRITE_STRING msg_d
    lda cnt
    jsr write_hex_byte

    ;lda W6522_REG_SR
    bra loop

; Writes hex byte in A to lcd
write_hex_byte:
    pha
    lsr
    lsr
    lsr
    lsr
    jsr write_hex_nybble
    pla
    and #%00001111
write_hex_nybble:
    tax
    lda hex_chars, x
    jmp hd44780_write_char

hex_chars .ascii '0123456789abcdef'

    .include "vectors.inc"
