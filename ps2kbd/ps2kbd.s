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

msg_sr0    .string "SR0: "
msg_ifr    .string " IFR: "
msg_sr1    .string "SCD: "
msg_cnt    .string " CNT: "

start:
    stz cnt
    W6522_DISABLE_INTERRUPTS W6522_IER_ALL

    jsr hd44780_init
    jsr ps2kbd_init

    cli     ; enable interrupts (I=0)

loop:
    HD44780_WRITE_STRING_AT msg_sr0, 0, 0
    lda sr0
    jsr write_hex_byte

    HD44780_WRITE_STRING msg_ifr
    lda ifr
    jsr write_hex_byte

    HD44780_WRITE_STRING_AT msg_sr1, 1, 0
    lda scancode
    jsr write_hex_byte

    HD44780_WRITE_STRING msg_cnt
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
