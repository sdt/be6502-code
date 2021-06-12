    .ifndef seen_ps2kbd
seen_ps2kbd .equ 1

    .include "6522.inc"
    .include "bit-count.inc"

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
    ZPB sr1
    ZPB scancode            ; also used for data out
    ZPB scancode_parity     ; parity of written data
    ZPB ifr                 ; must be zero page for BBR instructions
    ZPB output_bits         ; 8 bits to write
    ZPB ps2kbd_state

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
    PS2KBD_STATE PS2KBD_STATE_IDLE,             ps2kbd_state_idle

ps2kbd_init:
    STORE sr0, 0
    STORE sr1, 0
    STORE scancode, 0
    STORE ifr, 0
    ;lda #$ee    ; ps2 echo command - should response with EE 1110-1110
    ; f2 = 0100 1111
    ;    : 0000 1001    ab = 1101 0101  83 = 1100 0001
    ; 1011 0010
    ; D
    lda #$f2
    jsr ps2kbd_send_byte_host_to_device
    jsr ps2kbd_init_idle
    rts

ps2kbd_init_idle:
    W6522_SET_SR_MODE W6522_SR_MODE_IN_CB1
    W6522_SET_T2_MODE W6522_T2_MODE_COUNTDOWN_PB6
    W6522_SET_T2_COUNTER T2_COUNT   ; start=0,data*8,odd parity,stop=1

    stz W6522_REG_IFR
    W6522_ENABLE_INTERRUPTS W6522_IER_SR | W6522_IER_T2
    stz W6522_REG_SR                ; kick the shift register off

    PS2KBD_SET_STATE PS2KBD_STATE_IDLE
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
    rts


ps2kbd_fsm_step:
    ldx ps2kbd_state
    jmp (ps2kbd_state_table, x)

; This is the default state, just waiting for the keyboard to send us data.
ps2kbd_state_idle:
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
    lda bit_reverse_table, x    ; flip the bits around the right way
    sta scancode

    ; TODO - sort this out
    W6522_SET_SR_MODE W6522_SR_MODE_DISABLED    ; reset SR count?
    W6522_SET_SR_MODE W6522_SR_MODE_IN_CB1      ; reset SR count?
    stz W6522_REG_SR                            ; reset SR count? nope
.not_t2_interrupt:
    rts

PS2KBD_IRQ_HANDLER .macro
    lda W6522_REG_IFR           ; load interrupt flags
    sta ifr                     ; save them in ifr for bit tests
    jsr ps2kbd_fsm_step         ; call the fsm (so we have a return address)
    .endm

    .endif ; seen_ps2kbd
