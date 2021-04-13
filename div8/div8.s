    .dsect
    .org $0200

num  .db 0
den  .db 0
quo  .db 0
rem  .db 0
mask .db 0

    .dend

    .org $8000

reset:
    ldx #$ff
    txs

    lda #247
    sta num

    lda #13
    sta den

    jsr div8

end:
    jmp end

;   Q := 0                  -- Initialize quotient and remainder to zero
;   R := 0
;   for i := n − 1 .. 0 do  -- Where n is number of bits in N
;       R := R << 1         -- Left-shift R by 1 bit
;       R(0) := N(i)        -- Set the lsb bit of R equal to bit i of the numerator
;       if R ≥ D then
;           R := R − D
;           Q(i) := 1
;       end
;   end
div8:
    lda #0
    sta quo
    sta rem

    lda #%1000000
    sta mask
.loop:
    asl num
    rol rem         ; R = (R << 1) | N(i)
    lda rem
    cmp den
    bmi .skip       ; skip if rem < den
    sec             ; set carry before sub
    sbc den         ; rem -= den
    sta rem
    lda mask
    ora quo
    sta quo
.skip:
    clc
    ror mask
    bne .loop

    .include "vectors.inc"
