    .ifndef seen_ps2kbd
seen_ps2kbd .equ 1

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
    PS2KBD_STATE PS2KBD_STATE_WRITE_START_BIT,  ps2kbd_state_write_start_bit
    PS2KBD_STATE PS2KBD_STATE_WRITE_DATA_BITS,  ps2kbd_state_write_data_bits
    PS2KBD_STATE PS2KBD_STATE_WRITE_PARITY_BIT, ps2kbd_state_write_parity_bit
    PS2KBD_STATE PS2KBD_STATE_WRITE_STOP_BIT,   ps2kbd_state_write_stop_bit
    PS2KBD_STATE PS2KBD_STATE_DEVICE_ACK,       ps2kbd_state_device_ack

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
    lda #$0f    ; ps2 read ID command - should respond with AB 83
    bra ps2kbd_init_write

ps2kbd_init_idle:
    W6522_SET_SR_MODE W6522_SR_MODE_IN_CB1
    W6522_SET_T2_MODE W6522_T2_MODE_COUNTDOWN_PB6
    W6522_SET_T2_COUNTER T2_COUNT   ; start=0,data*8,odd parity,stop=1

    stz W6522_REG_IFR
    W6522_ENABLE_INTERRUPTS W6522_IER_SR | W6522_IER_T2
    stz W6522_REG_SR                ; kick the shift register off

    PS2KBD_SET_STATE PS2KBD_STATE_IDLE
    rts

; Call this to send a byte to the device. Data is in A register.
ps2kbd_init_write:
    sta scancode
    jsr parity
    eor #1 ; flip the bit for odd parity
    sta scancode_parity
    lda #8              ; 8 bits still to write
    sta output_bits

    ; Disable SR, set T2 mode to internal single shot.
    W6522_SET_SR_MODE W6522_SR_MODE_DISABLED
    W6522_SET_T2_MODE W6522_T2_MODE_SINGLE_SHOT

    ; Set the clock pin to output, pull the clock low
    PS2KBD_CLOCK_DDR PS2KBD_CLOCK_WRITE
    PS2KBD_SET_CLOCK PS2KBD_CLOCK_LOW

    ; Set the T2 timer for >= 100us
    W6522_SET_T2_COUNTER 100
    W6522_ENABLE_INTERRUPTS W6522_IER_T2

    ; Switch to begin write state
    PS2KBD_SET_STATE PS2KBD_STATE_WRITE_START_BIT
    rts

ps2kbd_fsm_step:
    ldx ps2kbd_state
    jmp (ps2kbd_state_table, x)

; This is the default state, just waiting for the keyboard to send us data.
ps2kbd_state_idle:
    ; Check SR interrupt for first 8 bits
    bbr W6552_IFR_BIT_SR, ifr, .not_sr_interrupt
    lda W6522_REG_SR            ; read SR (this resets the interrupt flag)
    sta sr0                     ; store first byte into sr0
.not_sr_interrupt:

    ; Check T2 interrupt for last 3 bits
    bbr W6552_IFR_BIT_T2, ifr, .not_t2_interrupt
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

; We've pulled the clock pin low, and set the T2 timer for 100us.
; When the T2 interrupt goes off, we write the start bit, and wait for the
; device to start clocking in data.
ps2kbd_state_write_start_bit:
    ; Check that it was in fact the T2 interrupt that went off.
    bbr W6552_IFR_BIT_T2, ifr, .not_t2_interrupt

    ; Pull the data pin CB2 low.
    W6522_SET_CB2_MODE W6522_CB2_MODE_LOW_OUT

    W6522_DISABLE_INTERRUPTS W6522_IER_T2   ; and disable the T2 interrupt

    ; Set the CB1 interrupt to the rising edge = CLK falling edge
    W6522_SET_CB1_MODE W6522_CB1_MODE_INPUT_RISING
    W6522_ENABLE_INTERRUPTS W6522_IER_CB1   ; and enable the CB1 interrupt

    PS2KBD_CLOCK_DDR PS2KBD_CLOCK_READ      ; release the clock pin (goes high)

    ; Now wait for the device to start clocking in bits
    PS2KBD_SET_STATE PS2KBD_STATE_WRITE_DATA_BITS

    lda W6522_REG_T2CL                      ; clear the T2 interrupt flag
.not_t2_interrupt:
    rts

; This is just way too slow. We need to have the data ready within 40us, but
; we're setting it at 60us.
ps2kbd_state_write_data_bits:
    ; Check that it was in fact the CB1 interrupt that went off.
    bbr W6552_IFR_BIT_CB1, ifr, .not_cb1_interrupt

    ; Shift the low data bit into the carry flag, and write it to CB2
    ror scancode
    bcs .write_one
.write_zero:
    W6522_SET_CB2_MODE W6522_CB2_MODE_LOW_OUT
    bra .write_done
.write_one:
    W6522_SET_CB2_MODE W6522_CB2_MODE_HIGH_OUT
.write_done:

    ; Decrement the counter, switch to the parity state if we hit zero
    dec output_bits
    bne .not_done
    PS2KBD_SET_STATE PS2KBD_STATE_WRITE_PARITY_BIT
.not_done:

    ; Clear the CB1 interrupt flag by reading ORB
    cmp W6522_REG_ORB

.not_cb1_interrupt:
    rts

ps2kbd_state_write_parity_bit:
    ; Check that it was in fact the CB1 interrupt that went off.
    bbr W6552_IFR_BIT_CB1, ifr, .not_cb1_interrupt

    ; Clear the CB1 interrupt flag by reading ORB
    cmp W6522_REG_ORB

    ; Shift the parity bit into the carry flag, and write it to CB2
    ror scancode_parity
    bcs .write_one
.write_zero:
    W6522_SET_CB2_MODE W6522_CB2_MODE_LOW_OUT
    bra .write_done
.write_one:
    W6522_SET_CB2_MODE W6522_CB2_MODE_HIGH_OUT
.write_done:

    ; Only one parity bit - switch to the write stop bit state
    PS2KBD_SET_STATE PS2KBD_STATE_WRITE_STOP_BIT

.not_cb1_interrupt:
    rts

ps2kbd_state_write_stop_bit:
    ; Check that it was in fact the CB1 interrupt that went off.
    bbr W6552_IFR_BIT_CB1, ifr, .not_cb1_interrupt

    ; Clear the CB1 interrupt flag by reading ORB
    cmp W6522_REG_ORB

    ; Release the data pin - set CB2 to input mode
    W6522_SET_CB2_MODE W6522_CB2_MODE_INPUT_FALLING

    ; We only need to wait for the clock rising edge now, CB1 falling
    W6522_SET_CB1_MODE W6522_CB1_MODE_INPUT_FALLING

    ; Nearly done - switch to the write stop bit state
    PS2KBD_SET_STATE PS2KBD_STATE_DEVICE_ACK

.not_cb1_interrupt:
    rts

ps2kbd_state_device_ack:
    ; Check that it was in fact the CB1 interrupt that went off.
    bbr W6552_IFR_BIT_CB1, ifr, .not_cb1_interrupt

    ; Clear the CB1 interrupt flag by reading ORB
    cmp W6522_REG_ORB

    ; No more CB1 interrupts needed. Disable them.
    W6522_DISABLE_INTERRUPTS W6522_IER_CB1

    ; And we're done - switch back to the idle state
    jmp ps2kbd_init_idle

.not_cb1_interrupt:
    rts



PS2KBD_IRQ_HANDLER .macro
    lda W6522_REG_IFR           ; load interrupt flags
    sta ifr                     ; save them in ifr for bit tests
    jsr ps2kbd_fsm_step         ; call the fsm (so we have a return address)
    .endm

    .endif ; seen_ps2kbd
