    .include "prelude.inc"
    .include "ps2kbd.inc"
    .include "terminal.inc"

    ZPB cnt

PS2KBD_CMD_QUEUE_BITS   = 3
PS2KBD_CMD_QUEUE_SIZE   = 1 << PS2KBD_CMD_QUEUE_BITS
PS2KBD_CMD_QUEUE_MASK   = PS2KBD_CMD_QUEUE_SIZE - 1

    .dsect
ps2kbd_cmd_queue reserve PS2KBD_CMD_QUEUE_SIZE
    .dend

    ZPB ps2kbd_cmd_queue_write_cursor
    ZPB ps2kbd_cmd_queue_read_cursor


; Command byte in A
ps2kdb_enqueue_cmd:
    ldx ps2kbd_cmd_queue_write_cursor   ; write cursor is in X
    sta ps2kbd_cmd_queue, x
    txa                                 ; copy the write cursor to` A
    inc
    and #PS2KBD_CMD_QUEUE_MASK
    sta ps2kbd_cmd_queue_write_cursor   ; update the write cursor via A
    cpx ps2kbd_cmd_queue_read_cursor    ; was this the first thing in the queue?
    beq ps2kbd_process_cmd_queue        ; if yes, continue on down to process it
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

    PS2KBD_IRQ_HANDLER

    plx
    pla
    rti

; 11 + 11 + 11 = 33 = 4 * 8 + 1

PS2KBD_CMD1 .macro b1
    lda #\b1
    jsr ps2kdb_enqueue_cmd
    .endm

PS2KBD_CMD2 .macro b1, b2
    PS2KBD_CMD1 \b1
    PS2KBD_CMD1 \b2
    .endm

start:
    STORE cnt, 0
    STORE ps2kbd_cmd_queue_write_cursor, 0
    STORE ps2kbd_cmd_queue_read_cursor, 0
    STORE prev_scancode, 0
    STORE leds, 0

    W6522_DISABLE_INTERRUPTS W6522_IER_ALL

    jsr ps2kbd_init
    jsr term_init

    cli     ; enable interrupts (I=0)

    PS2KBD_CMD1 $f6         ; set defaults
    PS2KBD_CMD2 $f3, $34    ; set typematics
    PS2KBD_CMD2 $ed, $00    ; set leds for some reason

;    ; Reset
;    ; > ff
;    ;  < fa
;    ;  ...
;    ;  < aa = good, fc = bad
;    lda #$ff ; reset
;    jsr ps2kdb_enqueue_cmd

;    ; Set defaults
;    ; > f6
;    ;  < fa
;    lda #$f6
;    jsr ps2kdb_enqueue_cmd

;    ; Set typematics
;    ; > f3
;    ;  < fa
;    ; > $arg
;    ;  < fa
;    lda #$f3
;    jsr ps2kdb_enqueue_cmd
;    lda #$7f
;    jsr ps2kdb_enqueue_cmd

;    ; Set LEDS
;    ; > ed
;    ;  < fa
;    ; > $leds
;    ;  > fa
;    lda #$ed
;    jsr ps2kdb_enqueue_cmd
;    lda #$3
;    jsr ps2kdb_enqueue_cmd

;    lda #$f0    ; set scan code set 2 (no set 3!)
;    jsr ps2kdb_enqueue_cmd
;    lda #$02
;    jsr ps2kdb_enqueue_cmd

;    ; Ask for scancode set
;    ; > f0
;    ;  < fa
;    ; > 00
;    ;  < fa
;    ;  < $set
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

    .dsect
prev_scancode .db 0
leds          .db 0
    .dend

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
    bne .not_fa

    ; ACK received from keyboard. Consume the last byte sent, and send another.
    lda ps2kbd_cmd_queue_read_cursor
    inc
    and #PS2KBD_CMD_QUEUE_MASK
    sta ps2kbd_cmd_queue_read_cursor
    jsr ps2kbd_process_cmd_queue
    bra loop

.not_fa:
    cmp #$fe
    bne .not_fe
    ; RESEND - resend the last command
    jsr ps2kbd_process_cmd_queue
    bra loop

.not_fe:
    cmp #$1c
    bne .not_a
    ldx prev_scancode
    cpx #$f0
    beq .not_a

    PS2KBD_CMD1 $ed
    lda leds
    inc
    and #7
    sta leds
    jsr ps2kdb_enqueue_cmd

    lda #$1c ; we know it was an 'a'

.not_a:
    sta prev_scancode
    bra loop

    .include "vectors.inc"
