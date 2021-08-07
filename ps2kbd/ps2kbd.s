    .include "prelude.inc"
    .include "ps2kbd.inc"
    .include "terminal.inc"

    .dsect
ps2kbd_cmd_queue reserve 256
    .dend

    ZPB ps2kbd_cmd_queue_write_cursor
    ZPB ps2kbd_cmd_queue_read_cursor
    ZPB modifier_keys

MOD_LEFTSHIFT   = %00000001
MOD_RIGHTSHIFT  = %00000010
MOD_SHIFT_MASK  = MOD_LEFTSHIFT | MOD_RIGHTSHIFT

; Command byte in A
ps2kdb_enqueue_cmd:
    ldx ps2kbd_cmd_queue_write_cursor
    sta ps2kbd_cmd_queue, x
    txa                                 ; stash the old write cursor in A
    inx
    stx ps2kbd_cmd_queue_write_cursor   ; update the write cursor
    cmp ps2kbd_cmd_queue_read_cursor    ; was this the first thing in the queue?
    bne .cmd_queue_busy
    jmp ps2kbd_process_cmd_queue

.cmd_queue_busy:
    rts

ps2kbd_process_cmd_queue:
    ldx ps2kbd_cmd_queue_read_cursor
    cpx ps2kbd_cmd_queue_write_cursor
    beq .cmd_queue_empty

    lda ps2kbd_cmd_queue, x

    pha
    lda #'('
    jsr term_write_char
    pla

    pha
    jsr term_write_hex_byte
    lda #')'
    jsr term_write_char

    pla
    jsr ps2kbd_send_byte_host_to_device

.cmd_queue_empty
    rts

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
    STORE ps2kbd_cmd_queue_write_cursor, 0
    STORE ps2kbd_cmd_queue_read_cursor, 0
    STORE modifier_keys, 0

    W6522_DISABLE_INTERRUPTS W6522_IER_ALL

    jsr ps2kbd_init
    jsr term_init

    cli     ; enable interrupts (I=0)

;    lda #$f0    ; set scan code set 2 (no set 3!)
;    jsr ps2kdb_enqueue_cmd
;    lda #$02
;    jsr ps2kdb_enqueue_cmd
;    lda #$f0
;    jsr ps2kdb_enqueue_cmd
;    lda #$00
;    jsr ps2kdb_enqueue_cmd
;    lda #$ed
;    jsr ps2kdb_enqueue_cmd
;    lda #$ed
;    jsr ps2kdb_enqueue_cmd
;    lda #$03
;    jsr ps2kdb_enqueue_cmd
;    lda #$f3
;    jsr ps2kdb_enqueue_cmd
;    lda #$7f
;    jsr ps2kdb_enqueue_cmd
;
loop:
    lda scancode_errors
    beq .no_errors
    dec scancode_errors
    lda #'X'
    jsr term_write_char
    bra loop

.no_errors:
    ldx scancode_queue_read_cursor
    cpx scancode_queue_write_cursor
    beq loop

    lda scancode_queue, x
    bmi .process_break

.process_make:

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
    bra .consume_key

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

.consume_key:
    inc scancode_queue_read_cursor
    bra loop

.set_modifier:
    ora modifier_keys
    sta modifier_keys
    inc scancode_queue_read_cursor
    bra loop

.clear_modifier:
    and modifier_keys
    sta modifier_keys
    inc scancode_queue_read_cursor
    bra loop

    .include "vectors.inc"
