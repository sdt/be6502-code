    .dsect
    .org $0200
    .dend

    .org $8000
reset:
    ldx #$ff
    txs

    lda #247
    sta num8

    lda #13
    sta den8

    jsr div8

end:
    jmp end

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
    .dsect
num8  .db 0
den8  .db 0
quo8  .db 0
rem8  .db 0
    .dend

div8:
    lda #0
    sta quo8        ; A holds rem8

    ldx #8
.loop:
    asl num8
    rol A           ; rem8 = (rem8 << 1) | num8[i]
    cmp den8        ; cmp rem8, den8
    bmi .skip       ; skip if rem8 < den8
    sbc den8        ; rem8 -= den8 ; c = 1 already
.skip:
    rol quo8        ; carry flag is set by bmi: quo8 = (quo8 << 1) | C
    dex
    bne .loop
    rts

test16:
    lda #<54321
    sta num16+0
    lda #>54321
    sta num16+1     ; num16 = 54321

    lda #<257
    sta den16+0
    lda #>257
    sta den16+1
    jsr div16
end16:
    jmp end16

    .dsect
num10 .dw 0         ; 16 bit numerator, 8 bit denominator = 10
quo10 .dw 0         ; 16 bit quotient
rem10 .db 0         ; but 8 bit remainer: (0..9)
   .dend

div10:
    lda #0          ; A holds rem10
    sta quo10+0
    sta quo10+1     ; quo10 = 0

    ldx #16
.loop:
    asl num10+0
    rol num10+1     ; num10 <<= 1 ; top bit of num16 now in carry
    rol A           ; rem10 = (rem10 << 1) | carry
    cmp #10
    bcc .r_lt_d     ; if r < d; do R < branch
    sbc #10         ; r -= d
.r_lt_d
    rol quo10+0
    rol quo10+1     ; quo10 = (quo10 << 1) | carry
    dex
    bne .loop
    sta rem10
    rts

    .dsect
num16 .dw 0
den16 .dw 0
quo16 .dw 0
rem16 .dw 0
    .dend

div16:
    lda #0
    sta rem16+0
    sta rem16+1     ; rem16 = 0
    sta quo16+0
    sta quo16+1     ; quo16 = 0

    ldx #16
.loop:
    asl num16+0
    rol num16+1     ; num16 <<= 1 ; top bit of num16 now in carry
    rol rem16+0
    rol rem16+1     ; rem16 = (rem16 << 1) | carry
    lda rem16+1
    cmp den16+1
    bcc .r_lt_d     ; if r.hi < d.hi -> do R <  D branch
    bne .r_ge_d     ; if r.hi > d.hi -> do R >= D branch
    lda rem16+0     ; r.hi == d.hi
    cmp den16+0
    bcc .r_lt_d     ; if r.lo < d.lo -> do R < D branch
.r_ge_d:
    sec             ; set carry ready for subtraction ; R -= D
    lda rem16+0
    sbc den16+0
    sta rem16+0
    lda rem16+1
    sbc den16+1
    sta rem16+1
    sec             ; set carry ready to shift into quo16
.r_lt_d:            ; if we jumped here, it was because carry was clear
    rol quo16+0
    rol quo16+1     ; Q = (Q << 1) | C
    dex
    bne .loop
    rts

    .dsect
itoa_in:    .dw 0
itoa_out:   .db 0, 0, 0, 0, 0, 0
    .dend

itoa:
    lda 0
    pha             ; push the string terminator onto the stack
    lda itoa_in+0
    sta num16+0
    lda itoa_in+1
    sta num16+1     ; num16 = itoa_in
    lda #10
    sta den16+0
    lda #0
    sta den16+1     ; den16 = 0
.loop:
    jsr div16       ; quo16 = N % 10
    lda rem16+0
    clc
    adc '0'
    pha             ; push ascii digit onto the stack
    lda quo16+0
    ora quo16+1
    beq .done       ; if quo16 is zero, we have all the digits on the stack
    lda quo16+0
    sta num16+0
    lda quo16+1
    sta num16+1     ; num16 = N / 10
    jmp .loop
.done
    ldx #$ff
.pop:
    inx             ; pre-increment to let pla set the z flag
    pla
    sta itoa_out,x
    bne .pop
    rts


    .include "vectors.inc"
