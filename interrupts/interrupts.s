    .include "prelude.inc"
    .include "6552.inc"
    .include "hd44780.inc"
    .include "itoa.inc"

    ZPW t1_count
    ZPW last_t1_count
    ZPW t2_count
    ZPW last_t2_count
    ZPB do_update

INCW .macro dest
    inc \dest+0
    bne .no_carry\@
    inc \dest+1
.no_carry\@:
    .endm

CMP16 .macro lhs, rhs
    lda \lhs+0
    cmp \rhs+0
    bne .different
    lda \lhs+1
    cmp \rhs+1
.different:
    .endm

CPY16 .macro src, dest
    lda \src+0
    sta \dest+0
    lda \src+1
    sta \dest+1
    .endm

STO16 .macro value, dest
    lda #<\value
    sta \dest+0
    lda #>\value
    sta \dest+1
    .endm

t1_msg .string "Timer 1: "
t2_msg .string "Timer 2: "

nmi:
    rti

irq:
    pha
    W6552_READ W6552_REG_IFR    ; load interrupt flags into A
    asl a                       ; bit 6 is T1 flag
    bmi .irq_t1
    asl a                       ; bit 5 is T2 flag
    bmi .irq_t2
.irq_done:
    pla
    rti

.irq_t1:
    W6552_ACK_T1        ; clear the IRQ
    INCW t1_count
    pla
    rti

.irq_t2:
    W6552_SET_T2_COUNTER 10000  ; reloading the counter clears the IRQ
    INCW t2_count
    pla
    rti

start:
    stz t1_count+0
    stz t1_count+1
    stz last_t1_count+0
    stz last_t1_count+1
    stz t2_count+0
    stz t2_count+1
    stz last_t2_count+0
    stz last_t2_count+1

    W6552_SET_T1_MODE W6552_T1_MODE_CONTINUOUS
    W6552_SET_T1_COUNTER 10000

    W6552_SET_T2_MODE W6552_T2_MODE_SINGLE_SHOT
    W6552_SET_T2_COUNTER 10000

    W6552_ENABLE_INTERRUPTS W6552_IER_T1|W6552_IER_T2

    jsr hd44780_init
    jsr update_display

    cli     ; enable interrupts (I=0)
loop:
    stz do_update

    sei
    CMP16 t1_count, last_t1_count
    beq irq_no_change
    CPY16 t1_count, last_t1_count
    inc do_update
irq_no_change:
    cli

    sei
    CMP16 t2_count, last_t2_count
    beq nmi_no_change
    CPY16 t2_count, last_t2_count
    inc do_update
nmi_no_change:
    cli

    lda do_update
    bne loop
    jsr update_display
    bra loop

update_display:
    HD44780_WRITE_STRING_AT t1_msg, 0, 0
    CPY16 last_t1_count, itoa_in
    jsr itoa
    jsr pad_itoa_out
    HD44780_WRITE_STRING itoa_out

    HD44780_WRITE_STRING_AT t2_msg, 1, 0
    CPY16 last_t2_count, itoa_in
    jsr itoa
    jsr pad_itoa_out
    HD44780_WRITE_STRING itoa_out
    rts

    ; pads itoa_out with spaces - assumes at least one digit is present
pad_itoa_out:
    ldx #0                  ; x = 1

.phase1:                    ; scan x forward until we reach the nul
    inx
    lda itoa_out,x
    bne .phase1

.phase2:                    ; write out spaces until x == 5
    txa
    cmp #5
    beq .done
    lda #' '
    sta itoa_out,x
    inx
    bra .phase2

.done:
    stz itoa_out,x          ; write out the nul
    rts

    .include "vectors.inc"
