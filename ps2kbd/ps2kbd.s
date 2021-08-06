    .include "prelude.inc"
    .include "ps2kbd.inc"
    .include "terminal.inc"

    ZPB cnt

    .dsect
ps2kbd_cmd_queue reserve 256
    .dend

    ZPB ps2kbd_cmd_queue_write_cursor
    ZPB ps2kbd_cmd_queue_read_cursor


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

    PS2KBD_IRQ_HANDLER

    ply
    plx
    pla
    rti

; 11 + 11 + 11 = 33 = 4 * 8 + 1

start:
    STORE cnt, 0
    STORE ps2kbd_cmd_queue_write_cursor, 0
    STORE ps2kbd_cmd_queue_read_cursor, 0

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
    lda #'X'
    jsr term_write_char
    bra loop

.no_errors:
    ldx scancode_queue_read_cursor
    cpx scancode_queue_write_cursor
    beq loop

    lda scancode_queue, x
    pha
    bmi .break
.make:
    lda #'D'
    jsr term_write_char
    lda #'n'
    jsr term_write_char
    pla
    bra .write_char
.break:
    lda #'U'
    jsr term_write_char
    lda #'p'
    jsr term_write_char
    pla
    and #$7f

.write_char:
    jsr term_write_hex_byte

    inc scancode_queue_read_cursor
    bra loop

    .include "vectors.inc"
