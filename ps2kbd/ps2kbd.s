    .include "prelude.inc"
    .include "ps2kbd.inc"
    .include "terminal.inc"

    ZPB modifier_keys
    ZPB leds

MOD_LEFTSHIFT   = %00000001
MOD_RIGHTSHIFT  = %00000010
MOD_SHIFT_MASK  = MOD_LEFTSHIFT | MOD_RIGHTSHIFT

irq:
    pha
    phx
    phy

    jsr ps2kbd_irq_handler

    ply
    plx
    pla
    rti

; 11 + 11 + 11 = 33 = 4 * 8 + 1

start:
    STORE modifier_keys, 0
    STORE leds, 0

    W6522_DISABLE_INTERRUPTS W6522_IER_ALL

    jsr ps2kbd_init
    jsr term_init

    cli     ; enable interrupts (I=0)

    jsr ps2kbd_set_defaults

loop:
    jsr ps2kbd_check_command_queue

    lda scancode_errors
    beq .no_errors
    stz scancode_errors
    jsr term_write_hex_byte
    ;dec scancode_errors
    ;lda #$f2
    ;jsr term_write_char
    bra loop
.no_errors:

    jsr ps2kbd_is_keycode_available
    beq loop
    jsr ps2kbd_read_keycode         ; keycode is in A
    bmi .process_break

.process_make:

    cmp #KEY_A
    bne .not_a
    pha
    lda leds
    eor #7
    sta leds
    jsr ps2kbd_set_leds
    pla
.not_a:

    cmp #KEY_LEFTSHIFT
    bne .not_leftshift
    lda #MOD_LEFTSHIFT
    bra .set_modifier
.not_leftshift:

    cmp #KEY_RIGHTSHIFT
    bne .not_rightshift
    lda #MOD_RIGHTSHIFT
    bra .set_modifier
.not_rightshift:

    tax
    lda modifier_keys
    and #MOD_SHIFT_MASK
    bne .shifted
    lda ps2kbd_plain_ascii, x
    bra .loaded
.shifted:
    lda ps2kbd_shifted_ascii, x
.loaded
    bne .write_char
    lda #TERM_UNKNOWN_CHAR
.write_char:
    jsr term_write_char
    bra loop

.process_break:
    cmp #(KEY_LEFTSHIFT|$80)
    bne .not_leftshift_break
    lda #(~MOD_LEFTSHIFT)
    bra .clear_modifier
.not_leftshift_break:

    cmp #(KEY_RIGHTSHIFT|$80)
    bne .not_rightshift_break
    lda #(~MOD_RIGHTSHIFT)
    bra .clear_modifier
.not_rightshift_break:

    bra loop

.set_modifier:
    ora modifier_keys
    sta modifier_keys
    bra loop

.clear_modifier:
    and modifier_keys
    sta modifier_keys
    bra loop

    .include "vectors.inc"
