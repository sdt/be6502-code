    .ifndef seen_ps2kbd
seen_ps2kbd .equ 1

    .include "6522.inc"
    .include "bit-count.inc"
    .include "bit-reverse.inc"
    .include "keycodes.inc"

; PS2 keyboard bits come in lsb-first
;
; K0   = Start bit = 0
; K1-8 = Data bits
; K9   = Odd parity
; KA   = Stop bit = 1
;
; SR like this:
;       76543210
; t=0   .......0
; t=1   ......01
; t=2   .....012
; ...
; t=7   01234567    ; SR interrupt fires - read SR resets this IRQ
; t=8   12345678
; t=9   23456789
; t=10  3456789A    ; T2 interrupt fires - reload T2 counter resets this

    ZPB sr0
    ZPB scancode            ; also used for data out
    ZPB scancode_parity     ; parity of written data
    ZPB ifr                 ; must be zero page for BBR instructions
    ZPB output_bits         ; 8 bits to write
    ZPB ps2kbd_state

KEYCODE_QUEUE_BITS = 6
KEYCODE_QUEUE_SIZE = 1 << KEYCODE_QUEUE_BITS
KEYCODE_QUEUE_MASK = KEYCODE_QUEUE_SIZE - 1

COMMAND_QUEUE_BITS = 3
COMMAND_QUEUE_SIZE = 1 << COMMAND_QUEUE_BITS
COMMAND_QUEUE_MASK = COMMAND_QUEUE_SIZE - 1

    ZPB keycode_queue_write_cursor
    ZPB keycode_queue_read_cursor
    ZPB command_queue_write_cursor
    ZPB command_queue_read_cursor

    .dsect
keycode_queue       .reserve KEYCODE_QUEUE_SIZE
command_queue       .reserve COMMAND_QUEUE_SIZE
scancode_errors     .db 0
command_response    .db 0
    .dend

T2_COUNT = 11 - 1           ; one less because that's how t2 011 mode works

; A 1C -> 0001 1100 -> 0:0011:1000:0:1 -> 0011:1000=78 + 1000:0111=
;                      0 1234 5678 9 A
;
; t=0   .......0  ; start bit=0
; t=1   ......00
; t=2   .....000
; t=3   ....0001
; t=4   ...00011  ; s,0xC,
; t=5   ..000111
; t=6   .0001110
; t=7   00011100
;---
; t=8   00111000  ; 0xC,0x1
; t=9   01110000  ;
; t=a   11100001  ; stop bit

; 0438  0100 0011 1000
;        SP 0001 1100 s   1c

PS2KBD_CLOCK_REG_OR     = W6522_REG_ORB
PS2KBD_CLOCK_REG_IR     = W6522_REG_IRB
PS2KBD_CLOCK_REG_DDR    = W6522_REG_DDRB
PS2KBD_CLOCK_MASK       = %10000000   ; PB7
PS2KBD_CLOCK_READ       = 0
PS2KBD_CLOCK_WRITE      = PS2KBD_CLOCK_MASK
PS2KBD_CLOCK_LOW        = 0
PS2KBD_CLOCK_HIGH       = PS2KBD_CLOCK_MASK

PS2KBD_SET_CLOCK .macro value
    W6522_WRITE_MASKED PS2KBD_CLOCK_REG_OR, \value, PS2KBD_CLOCK_MASK
    .endm

PS2KBD_CLOCK_DDR .macro value
    W6522_WRITE_MASKED PS2KBD_CLOCK_REG_DDR, \value, PS2KBD_CLOCK_MASK
    .endm

PS2KBD_STATE .macro name, handler
\name = * - ps2kbd_state_table  ; offset from the start of the table: 0,2,4...
    .dw \handler
    .endm

PS2KBD_SET_STATE .macro name
    STORE ps2kbd_state, \name
    .endm

ps2kbd_state_table:
    PS2KBD_STATE    PS2KBD_STATE_STD,       ps2kbd_state_std
    PS2KBD_STATE    PS2KBD_STATE_STD_BRK,   ps2kbd_state_std_brk
    PS2KBD_STATE    PS2KBD_STATE_EXT0,      ps2kbd_state_ext0
    PS2KBD_STATE    PS2KBD_STATE_EXT0_BRK,  ps2kbd_state_ext0_brk
    PS2KBD_STATE    PS2KBD_STATE_EXT1,      ps2kbd_state_ext1
    PS2KBD_STATE    PS2KBD_STATE_EXT1_BRK,  ps2kbd_state_ext1_brk

ps2kbd_init:
    STORE sr0, 0
    STORE scancode, 0
    STORE scancode_errors, 0
    STORE command_response, 0
    STORE ifr, 0
    STORE keycode_queue_write_cursor, 0
    STORE keycode_queue_read_cursor, 0
    STORE command_queue_write_cursor, 0
    STORE command_queue_read_cursor, 0
    PS2KBD_SET_STATE PS2KBD_STATE_STD

    ; Fall through to ps2kbd_init_idle

ps2kbd_init_idle:
    W6522_SET_SR_MODE W6522_SR_MODE_IN_CB1
    W6522_SET_T2_MODE W6522_T2_MODE_COUNTDOWN_PB6
    W6522_SET_T2_COUNTER T2_COUNT   ; start=0,data*8,odd parity,stop=1

    stz W6522_REG_IFR
    W6522_ENABLE_INTERRUPTS W6522_IER_SR | W6522_IER_T2
    stz W6522_REG_SR                ; kick the shift register off

    rts

; Assume that most recent scancode is in Y register
ps2kbd_state_std:
    cpy #$fa                        ; was is an ack
    beq register_ack_nak            ; --> register the ack
;    cpy #$fe                        ; was is a nak
;    beq register_ack_nak            ; --> register the nak
    cpy #$f0                        ; was it a break code?
    beq enter_ps2kbd_state_std_brk  ; -> enter std_brk state
    cpy #$e0                        ; was it an ext0 code?
    beq enter_ps2kbd_state_ext0     ; -> enter ext0 state
    cpy #$e1                        ; was it an ext1 code?
    beq enter_ps2kbd_state_ext1     ; -> enter ext1 state
    lda ps2kbd_standard_keycode, y  ; load the keycode
    beq ps2kbd_scancode_error       ; handle error (keycode = 0)
    bra ps2kbd_emit_keycode         ; emit the make keycode

register_ack_nak:
    sty command_response
    rts

ps2kbd_state_std_brk:
    lda ps2kbd_standard_keycode, y
    beq ps2kbd_scancode_error
    ora #$80
    bra ps2kbd_emit_keycode         ; emit the make keycode

ps2kbd_state_ext0:
    cpy #$f0                        ; was is a break code?
    beq enter_ps2kbd_state_ext0_brk  ; -> enter std_brk state
    cpy #$12                        ; was it the weird numlock/shift thing?
    beq enter_ps2kbd_state_std      ; --> ignore it
    lda ps2kbd_extended_keycode, y  ; load the keycode
    beq ps2kbd_scancode_error       ; handle error (keycode = 0)
    bra ps2kbd_emit_keycode         ; emit the make keycode

ps2kbd_state_ext0_brk:
    cpy #$12                        ; was it the weird numlock/shift thing?
    beq enter_ps2kbd_state_std      ; --> ignore it
    lda ps2kbd_extended_keycode, y
    beq ps2kbd_scancode_error
    ora #$80
    bra ps2kbd_emit_keycode         ; emit the make keycode

ps2kbd_state_ext1:
    cpy #$f0                        ; was is a break code?
    beq enter_ps2kbd_state_ext1_brk ; -> enter std_brk state
    cpy #$14                        ; was it the 14 in the middle?
    beq ps2kbd_ignore               ; -> ignore it
    cpy #$77                        ; was it the final 77?
    bne ps2kbd_scancode_error       ; -> if not error
    lda #KEY_PAUSE
    bra ps2kbd_emit_keycode         ; emit the make keycode for pause

ps2kbd_state_ext1_brk:
    cpy #$14                        ; was it the 14 in the middle?
    beq enter_ps2kbd_state_ext1     ; -> ignore it
    cpy #$77                        ; was it the final 77?
    bne ps2kbd_scancode_error       ; -> if not error
    lda #(KEY_PAUSE | $80)
    bra ps2kbd_emit_keycode         ; emit the break keycode for pause

; Keycode is in A
ps2kbd_emit_keycode:
    ldx keycode_queue_write_cursor
    sta keycode_queue, x
    txa
    inc
    and #KEYCODE_QUEUE_MASK
    sta keycode_queue_write_cursor
    PS2KBD_SET_STATE PS2KBD_STATE_STD   ; revert to the std stte
ps2kbd_ignore:
    rts

ps2kbd_scancode_error:
    sty scancode_errors
;    inc scancode_errors                 ; "log" the error, fall through
enter_ps2kbd_state_std:
    PS2KBD_SET_STATE PS2KBD_STATE_STD   ; revert to the std state
    rts

enter_ps2kbd_state_std_brk:
    PS2KBD_SET_STATE PS2KBD_STATE_STD_BRK
    rts

enter_ps2kbd_state_ext0:
    PS2KBD_SET_STATE PS2KBD_STATE_EXT0
    rts

enter_ps2kbd_state_ext0_brk:
    PS2KBD_SET_STATE PS2KBD_STATE_EXT0_BRK
    rts

enter_ps2kbd_state_ext1:
    PS2KBD_SET_STATE PS2KBD_STATE_EXT1
    rts

enter_ps2kbd_state_ext1_brk:
    PS2KBD_SET_STATE PS2KBD_STATE_EXT1_BRK
    rts

; Clear the mask in the IFR register, then wait for any of the mask bits to be
; set. Then clear it again.
WAIT_FOR_IFR_ANY .macro mask
    W6522_WRITE_MASKED W6522_REG_IFR, 0, \mask
    lda #\mask
.loop\@:
    bit W6522_REG_IFR
    beq .loop\@
    W6522_WRITE_MASKED W6522_REG_IFR, 0, \mask
    .endm

; Wait for the clock to be high, then low.
; Assumes PS2KBD_CLOCK_DDR PS2KBD_CLOCK_READ
ps2kbd_wait_for_falling_clock:
    lda #PS2KBD_CLOCK_MASK
.wait_for_high:
    bit PS2KBD_CLOCK_REG_IR
    beq .wait_for_high
.wait_for_low:
    bit PS2KBD_CLOCK_REG_IR
    bne .wait_for_low
    rts

; Wait for the clock to be low, then high.
; Assumes PS2KBD_CLOCK_DDR PS2KBD_CLOCK_READ
ps2kbd_wait_for_rising_clock:
    lda #PS2KBD_CLOCK_MASK
.wait_for_low:
    bit PS2KBD_CLOCK_REG_IR
    bne .wait_for_low
.wait_for_high:
    bit PS2KBD_CLOCK_REG_IR
    beq .wait_for_high
    rts

; Call this to send a byte to the device. Data is in A register.
ps2kbd_send_byte_host_to_device:
    sta scancode

; Show the outgoing commands
;    lda #'<'
;    jsr term_write_char
;    lda scancode
;    jsr term_write_hex_byte
;    lda #'>'
;    jsr term_write_char
;    lda scancode

    jsr parity
    eor #1 ; flip the bit for odd parity
    sta scancode_parity
    lda #8              ; 8 bits still to write
    sta output_bits

    ; Set the clock pin to output, pull the clock low
    PS2KBD_CLOCK_DDR PS2KBD_CLOCK_WRITE
    PS2KBD_SET_CLOCK PS2KBD_CLOCK_LOW

    ; Disable SR, set T2 mode to internal single shot. Disable interrupts.
    W6522_SET_SR_MODE W6522_SR_MODE_DISABLED
    W6522_SET_T2_MODE W6522_T2_MODE_SINGLE_SHOT
    W6522_DISABLE_INTERRUPTS W6522_IER_SR | W6522_IER_T2

    ; Set the T2 timer for >= 100us
    W6522_SET_T2_COUNTER 100

    ; Wait for the timer bit to be set.
    WAIT_FOR_IFR_ANY W6522_IFR_T2

    ; Pull the data pin CB2 low.
    W6522_SET_CB2_MODE W6522_CB2_MODE_LOW_OUT

    ; Release the CLK (goes high) - this is the start bit.
    PS2KBD_CLOCK_DDR PS2KBD_CLOCK_READ

    ; Set the CB1 IFR flag on CB1 rising edge (CLK falling)
    W6522_SET_CB1_MODE W6522_CB1_MODE_INPUT_RISING

.data_parity_bits:

    ; Wait for the clock line to go low
    jsr ps2kbd_wait_for_falling_clock ; WAIT_FOR_IFR_ANY W6522_IFR_CB1

    ror scancode
    bcs .write_one
.write_zero:
    ; Pull the data pin CB2 low.
    W6522_SET_CB2_MODE W6522_CB2_MODE_LOW_OUT
    bra .write_done
.write_one:
    ; Release the data pin CB2 - let it be pulled high
    W6522_SET_CB2_MODE W6522_CB2_MODE_INPUT_FALLING
.write_done:

    ; Decrement the bit counter.
    dec output_bits
    bmi .stop_bit           ; if the counter is negative, we're done
    bne .data_parity_bits   ; if the count is positive, keep going
    ; After 8 bits (ie counter is now zero), shift in the parity bit, for
    ; one more go around.
    lda scancode_parity
    sta scancode
    bra .data_parity_bits

.stop_bit:
    ; Wait for the clock line to go low
    jsr ps2kbd_wait_for_falling_clock ; WAIT_FOR_IFR_ANY W6522_IFR_CB1

    ; Release the data line
    W6522_SET_CB2_MODE W6522_CB2_MODE_INPUT_FALLING

.ack:
    jsr ps2kbd_wait_for_rising_clock
    jmp ps2kbd_init_idle    ; return

; Call this in your IRQ handler. Trashes A, X and Y.
ps2kbd_irq_handler:
    lda W6522_REG_IFR           ; load interrupt flags
    sta ifr                     ; save them in ifr for bit tests

    ; Check SR interrupt for first 8 bits
    bbr W6522_IFR_BIT_SR, ifr, .not_sr_interrupt
    lda W6522_REG_SR            ; read SR (this resets the interrupt flag)
    sta sr0                     ; store first byte into sr0
.not_sr_interrupt:

    ; Check T2 interrupt for last 3 bits
    bbr W6522_IFR_BIT_T2, ifr, .not_t2_interrupt
    W6522_SET_T2_COUNTER T2_COUNT ; restart T2 timer (this resets the IRQ)
    lda W6522_REG_SR            ; read SR
    ror a                       ; drop the stop bit
    ror a                       ; drop the parity bit
    ror a                       ; store the 8th bit in carry
    lda sr0                     ; load the first byte
    rol a                       ; rotate the carry bit back in
    tax
    ldy bit_reverse_table, x    ; flip the bits around the right way

    jsr .ps2kbd_fsm_step
    ; Reset SR - we seem to need all three of these
    W6522_SET_SR_MODE W6522_SR_MODE_DISABLED    ; reset SR count?
    W6522_SET_SR_MODE W6522_SR_MODE_IN_CB1      ; reset SR count?
    stz W6522_REG_SR                            ; reset SR count? nope
.not_t2_interrupt:
    rts

.ps2kbd_fsm_step:
    ldx ps2kbd_state
    jmp (ps2kbd_state_table, x)

; Sets Z flag is no keycodes available, returns read cursor in X
ps2kbd_is_keycode_available:
    ldx keycode_queue_read_cursor
    cpx keycode_queue_write_cursor
    rts

; Assumes ps2kbd_keycode_available has just been called, and the X register
; still holds keycode_queue_read_cursor
ps2kbd_read_keycode:
    txa                             ; use the A register to increment and
    inc                             ; wrap the read cursor
    and #KEYCODE_QUEUE_MASK
    sta keycode_queue_read_cursor
    lda keycode_queue, x            ; load A from the old read cursor in X
    rts

PS2KBD_ENQUEUE .macro cmd_or_arg
    lda #\cmd_or_arg
    jsr ps2kbd_enqueue_command
    .endm

ps2kbd_set_defaults:
    PS2KBD_ENQUEUE $f6  ; set defaults
    PS2KBD_ENQUEUE $f3  ; set typematic
    PS2KBD_ENQUEUE $34  ; 5.0cps, 500ms delay
    jmp ps2kbd_process_cmd_queue

; Led state in A
ps2kbd_set_leds:
    pha
    PS2KBD_ENQUEUE $ed ; set leds
    pla
    jsr ps2kbd_enqueue_command
    jmp ps2kbd_process_cmd_queue

; Command byte in A
ps2kbd_enqueue_command:
    ldx command_queue_write_cursor   ; write cursor is in X
    sta command_queue, x
    txa                              ; copy the write cursor to` A
    inc
    and #COMMAND_QUEUE_MASK
    sta command_queue_write_cursor   ; update the write cursor via A
    rts

ps2kbd_check_command_queue:
    sei                     ; command_response gets written from ISR
    lda command_response
    stz command_response
    cli
    bne .check_ack
    rts
.check_ack
    cmp #$fa
    bne ps2kbd_process_cmd_queue ; if not ack, skip straight to process queue
    ; consume the command
    lda command_queue_read_cursor
    inc
    and #COMMAND_QUEUE_MASK
    sta command_queue_read_cursor
    ; fall through to process command queue

ps2kbd_process_cmd_queue:
    ldx command_queue_read_cursor
    cpx command_queue_write_cursor
    beq .no_command_available
    lda command_queue, x
    jsr ps2kbd_send_byte_host_to_device
.no_command_available:
    rts

    .endif ; seen_ps2kbd
