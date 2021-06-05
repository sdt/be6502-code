    .ifndef seen_ps2kbd
seen_ps2kbd .equ 1

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
    ZPB scancode
    ZPB ifr ; must be zero page for BBR instructions
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
; t=9   01110000  ; parity=0
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
    PS2KBD_STATE PS2KBD_STATE_IDLE, ps2kbd_state_idle
    PS2KBD_STATE PS2KBD_STATE_BEGIN_WRITE, ps2kbd_state_inhibit
    PS2KBD_STATE PS2KBD_STATE_SEND_DATA_BITS, ps2kbd_state_send_data_bits
    PS2KBD_STATE PS2KBD_STATE_SEND_PARITY_STOP, ps2kbd_state_send_parity_stop

ps2kbd_init:
    STORE sr0, 0
    STORE sr1, 0
    STORE scancode, 0
    STORE ifr, 0
    PS2KBD_SET_STATE PS2KBD_STATE_BEGIN_WRITE
    bra ps2kbd_init_write

ps2kbd_init_idle:
    W6522_SET_SR_MODE W6522_SR_MODE_IN_CB1
    W6522_SET_T2_MODE W6522_T2_MODE_COUNTDOWN_PB6
    W6522_SET_T2_COUNTER T2_COUNT   ; start=0,data*8,odd parity,stop=1

    stz W6522_REG_IFR
    W6522_ENABLE_INTERRUPTS W6522_IER_SR | W6522_IER_T2
    stz W6522_REG_SR                ; kick the shift register off
    rts

ps2kbd_init_write:
    ; Disable SR, set T2 mode to internal single shot.
    W6522_SET_SR_MODE W6522_SR_MODE_DISABLED
    W6522_SET_T2_MODE W6522_T2_MODE_SINGLE_SHOT

    ; Set the clock pin to output, pull the clock low
    PS2KBD_CLOCK_DDR PS2KBD_CLOCK_WRITE
    PS2KBD_SET_CLOCK PS2KBD_CLOCK_LOW

    ; Set the T2 timer for >= 100us
    W6522_SET_T2_COUNTER 100
    W6522_ENABLE_INTERRUPTS W6522_IER_T2
    rts

ps2kbd_fsm_step:
    ldx ps2kbd_state
    jmp (ps2kbd_state_table, x)

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

    W6522_SET_SR_MODE W6522_SR_MODE_DISABLED    ; reset SR count?
    W6522_SET_SR_MODE W6522_SR_MODE_IN_CB1      ; reset SR count?
    stz W6522_REG_SR                            ; reset SR count? nope
.not_t2_interrupt:
    rts

ps2kbd_state_inhibit:
    ; Check that it was in fact the T2 interrupt that went off.
    bbr W6552_IFR_BIT_T2, ifr, .not_t2_interrupt

    ; pull the data pin low by writing 0's out of the shift register
    ; This is the only way I can find to keep the data pin low after starting
    ; the W6522_SR_MODE_OUT_CB1 below. I tried using the CB1 control to set
    ; it low, but it seems a bit random what it does between that, and setting
    ; the SR mode.
    W6522_SET_SR_MODE W6522_SR_MODE_OUT_PHI2
    lda #0
    sta W6522_REG_SR                        ; write command to SR

    lda W6522_REG_T2CL                  ; clear the T2 interrupt flag
    W6522_DISABLE_INTERRUPTS W6522_IER_T2 ; and disable the T2 interrupt

    PS2KBD_CLOCK_DDR PS2KBD_CLOCK_READ  ; release the clock pin (goes high)

    W6522_SET_SR_MODE W6522_SR_MODE_OUT_CB1 ; set SR to output mode
    lda #$77                                ; EE = ps/2 echo request (reversed)
    sta W6522_REG_SR                        ; write command to SR
    W6522_ENABLE_INTERRUPTS W6522_IER_SR    ; enable SR interrupt

    PS2KBD_SET_STATE PS2KBD_STATE_SEND_DATA_BITS
.not_t2_interrupt:
    rts

ps2kbd_state_send_data_bits:
    ; Check that it was in fact the SR interrupt that went off.
    bbr W6552_IFR_BIT_SR, ifr, .not_sr_interrupt
    W6522_DISABLE_INTERRUPTS W6522_IER_SR   ; disable SR interrupt

    ; We need to send two more data bits
    W6522_SET_T2_MODE W6522_T2_MODE_COUNTDOWN_PB6
    ;W6522_SET_T2_COUNTER 2 - 1              ; two (-1 for reasons)
    W6522_SET_T2_COUNTER 1
    W6522_ENABLE_INTERRUPTS W6522_IER_T2
    lda #%11000000                          ; parity=1 & stop bit=1
    sta W6522_REG_SR
    PS2KBD_SET_STATE PS2KBD_STATE_SEND_PARITY_STOP
.not_sr_interrupt:
    rts

ps2kbd_state_send_parity_stop:
    ; Check that it was in fact the T2 interrupt that went off.
    bbr W6552_IFR_BIT_T2, ifr, .not_t2_interrupt

    W6522_SET_SR_MODE W6522_SR_MODE_DISABLED    ; stop shifting bits out
    ;W6522_SET_CB2_MODE W6522_CB2_MODE_INPUT_FALLING
    lda W6522_REG_T2CL                      ; clear the T2 interrupt flag
    W6522_DISABLE_INTERRUPTS W6522_IER_T2   ; and disable the T2 interrupt
.not_t2_interrupt:
    rts

PS2KBD_IRQ_HANDLER .macro
    lda W6522_REG_IFR           ; load interrupt flags
    sta ifr                     ; save them in ifr for display
    jsr ps2kbd_fsm_step         ; call the fsm (so we have a return address)
    .endm

    .endif ; seen_ps2kbd
