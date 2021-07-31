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
    lda #'>'
    jsr term_write_char
    pla

    pha
    jsr term_write_hex_byte
    lda #'<'
    jsr term_write_char

    pla
    jsr ps2kbd_send_byte_host_to_device

.cmd_queue_empty
    rts

irq:
    pha
    phx

    PS2KBD_IRQ_HANDLER

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

    ;lda #$f2
    ;jsr ps2kdb_enqueue_cmd
    ;jsr ps2kbd_send_byte_host_to_device

loop:
    ldx scancode_queue_read_cursor
    cpx scancode_queue_write_cursor
    beq loop

    lda scancode_queue, x
    pha
    jsr term_write_hex_byte

    inc scancode_queue_read_cursor

    pla
    cmp #$fa
    bne .not_ack

    ; ACK received from keyboard. Consume the last byte sent, and send another.
;    inc ps2kbd_cmd_queue_read_cursor
;    jsr ps2kbd_process_cmd_queue

.not_ack:
    bra loop

    .include "vectors.inc"
