    .include "prelude.inc"
    .include "ps2kbd.inc"
    .include "terminal.inc"

    ZPB modifier_keys
    ZPB leds

MOD_LEFTSHIFT   = %00000001
MOD_RIGHTSHIFT  = %00000010
MOD_SHIFT_MASK  = MOD_LEFTSHIFT | MOD_RIGHTSHIFT

LED_SCROLLLOCK  = %00000001
LED_NUMLOCK     = %00000010
LED_CAPSLOCK    = %00000100

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

msg: .ascii ">"

start:
    STORE modifier_keys, 0
    STORE leds, 0

    W6522_DISABLE_INTERRUPTS W6522_IER_ALL

    jsr ps2kbd_init
    jsr term_init

    cli     ; enable interrupts (I=0)

    TERM_WRITE_STRING msg

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

    cmp #KEY_CAPSLOCK
    bne .not_capslock
    lda #LED_CAPSLOCK
    bra .toggle_leds
.not_capslock:

    cmp #KEY_NUMLOCK
    bne .not_numlock
    lda #LED_NUMLOCK
    bra .toggle_leds
.not_numlock:

    cmp #KEY_SCROLLLOCK
    bne .not_scrolllock
    lda #LED_SCROLLLOCK
    bra .toggle_leds
.not_scrolllock:

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
    ldy ps2kbd_plain_ascii, x
    bra .loaded
.shifted:
    ldy ps2kbd_shifted_ascii, x
.loaded:
    bne .known_char
    lda #TERM_UNKNOWN_CHAR
    jsr term_write_char
    bra loop
.known_char:
    lda #LED_CAPSLOCK
    and leds
    bne .capslock_on
    tya
    jsr term_write_char
    bra loop
.capslock_on:
    tya
    sec
    sbc #'A'        ; char = char - 'A'
    and #( ~$20 )   ; char &= ~0x20
    cmp #26         ; if (char < 26)
    bcs .not_alpha
    tya
    eor #$20        ;   char ^= 0x20 ; flip case
    jsr term_write_char
    bra loop
.not_alpha:
    tya
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
    jmp loop

.set_modifier:
    ora modifier_keys
    sta modifier_keys
    jmp loop

.clear_modifier:
    and modifier_keys
    sta modifier_keys
    jmp loop

; led bit to toggle is in A
.toggle_leds:
    eor leds
    sta leds
    jsr ps2kbd_set_leds
    jmp loop

    .include "vectors.inc"
