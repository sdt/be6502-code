    .include "prelude.inc"
    .include "6552.inc"
    .include "hd44780.inc"

    ZPW     hd44780_string
    ZPW     itoa_in
    ZPDATA  itoa_out, 6

start:
    hd44780_init

    hd44780_write_register $38 ; 8-bit interface, 2-line display, 5x8 fnot
    hd44780_write_register $01 ; clear
    hd44780_write_register $02 ; home
    hd44780_write_register $0e ; display on, cursor on, blink off

    ;hd44780_write_string msg0, 0

    lda #250
    sta itoa_in+0
    lda #0
    sta itoa_in+1

loop:
    jsr itoa

    hd44780_write_string itoa_out, 0

INCW .macro arg
    inc \arg+0
    bne .no_carry\@
    inc \arg+1
.no_carry\@:
    .endm

    INCW itoa_in
    jsr itoa

    hd44780_write_string itoa_out, 1

    INCW itoa_in
    bra loop

itoa:
    lda #0
    pha             ; push the string terminator onto the stack
    lda itoa_in+0
    sta num10+0
    lda itoa_in+1
    sta num10+1     ; num10 = itoa_in
.loop:
    jsr div10       ; quo16 = N / 10 ; A = N % 10
    clc
    adc #'0'        ; A = (N % 10) + '0'
    pha             ; push ascii digit onto the stack
    lda num10+0
    ora num10+1
    beq .done       ; if quo16 is zero, we have all the digits on the stack
    jmp .loop
.done
    ldx #$ff        ; pop the digits off the stack into itoa_out
.pop:               ; including the zero terminator
    inx             ; pre-increment to let pla set the z flag
    pla
    sta itoa_out,x
    bne .pop
    rts

;   Q := 0                  -- Initialize quotient and remainder to zero
;   R := 0
;   for i := n − 1 .. 0 do  -- Where n is number of bits in N
;       R := R << 1         -- Left-shift R by 1 bit
;       R(0) := N(i)        -- Set lsb of R equal to bit i of the numerator
;       if R ≥ D then
;           R := R − D
;           Q(i) := 1
;       end
;   end

    ZPW num10           ; 16 bit numerator in, 16 bit quotient out


; INPUT:
;   num10: 16 bit unsigned numerator
; OUTPUT:
;   num10: 16 bit unsigned quotient
;   reg A:  8 bit unsigned remainder
;   reg X;  zero
;   num10:  zero
div10:
    lda #0          ; A holds rem10
    ldx #16
.loop:
    asl num10+0
    rol num10+1     ; num10 <<= 1 ; top bit of num16 now in carry
    rol A           ; rem10 = (rem10 << 1) | carry
    cmp #10
    bcc .r_lt_d     ; if r < d; do branch
    sbc #10         ; r -= d (carry is already set)
    smb 0,num10+0   ; num10 |= 1
.r_lt_d
    dex
    bne .loop
    rts

    .include "vectors.inc"
