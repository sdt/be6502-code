    .include "prelude.inc"
    .include "6522.inc"
    .include "hd44780.inc"
    .include "bit-reverse.inc"

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
    ZPB cnt

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


T2_COUNT = 11 - 1           ; one less because that's how t2 011 mode works


irq:
    pha
    phx

    lda W6522_REG_IFR           ; load interrupt flags
    sta ifr                     ; save them in ifr for display

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

    plx
    pla
    inc cnt

    rti

; 11 + 11 + 11 = 33 = 4 * 8 + 1

msg_sr0    .string "SR0: "
msg_ifr    .string " IFR: "
msg_sr1    .string "SCD: "
msg_cnt    .string " CNT: "

start:

    stz sr0
    stz sr1
    stz scancode
    stz ifr
    stz cnt
    jsr hd44780_init

    W6522_DISABLE_INTERRUPTS W6522_IER_ALL
    ;W6522_ENABLE_INTERRUPTS W6522_IER_SR

    W6522_SET_SR_MODE W6522_SR_MODE_IN_CB1
    W6522_SET_T2_MODE W6522_T2_MODE_COUNTDOWN_PB6
    W6522_SET_T2_COUNTER T2_COUNT   ; start=0,data*8,odd parity,stop=1

    stz W6522_REG_IFR
    W6522_ENABLE_INTERRUPTS W6522_IER_SR | W6522_IER_T2
    stz W6522_REG_SR                ; kick the shift register off

    cli     ; enable interrupts (I=0)
loop:
    HD44780_WRITE_STRING_AT msg_sr0, 0, 0
    lda sr0
    jsr write_hex_byte

    HD44780_WRITE_STRING msg_ifr
    lda ifr
    jsr write_hex_byte

    HD44780_WRITE_STRING_AT msg_sr1, 1, 0
    lda scancode
    jsr write_hex_byte

    HD44780_WRITE_STRING msg_cnt
    lda cnt
    jsr write_hex_byte

    ;lda W6522_REG_SR
    bra loop

; Writes hex byte in A to lcd
write_hex_byte:
    pha
    lsr
    lsr
    lsr
    lsr
    jsr write_hex_nybble
    pla
    and #%00001111
write_hex_nybble:
    tax
    lda hex_chars, x
    jmp hd44780_write_char

hex_chars .ascii '0123456789abcdef'

    .include "vectors.inc"
