    .include "prelude.inc"
    .include "6552.inc"
    .include "hd44780.inc"
    .include "itoa.inc"

    ZPW irq_count
    ZPW last_irq_count
    ZPW nmi_count
    ZPW last_nmi_count
    ZPB do_update

INCW .macro dest
    inc \dest+0
    bne .no_carry
    inc \dest+1
.no_carry:
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

nmi_msg .string "NMI: "
irq_msg .string "IRQ: "
spaces  .string "    "

nmi:
    pha
    INCW nmi_count
    pla
    rti

irq:
    pha
    INCW irq_count
    pla
    rti

start:
    stz irq_count+0
    stz irq_count+1
    stz last_irq_count+0
    stz last_irq_count+1
    stz nmi_count+0
    stz nmi_count+1
    stz last_nmi_count+0
    stz last_nmi_count+1

    jsr hd44780_init
    jsr update_display

    cli     ; enable interrupts (I=0)
loop:
    stz do_update

    sei
    CMP16 irq_count, last_irq_count
    beq irq_no_change
    CPY16 irq_count, last_irq_count
    inc do_update
irq_no_change:
    cli

    sei
    CMP16 nmi_count, last_nmi_count
    beq nmi_no_change
    CPY16 nmi_count, last_nmi_count
    inc do_update
nmi_no_change:
    cli

    lda do_update
    bne loop
    jsr update_display
    bra loop

update_display:
    HD44780_WRITE_STRING_AT irq_msg, 0, 0
    CPY16 last_irq_count, itoa_in
    jsr itoa
    jsr pad_itoa_out
    STO16 itoa_out, hd44780_string
    jsr hd44780_write_string

    HD44780_WRITE_STRING_AT nmi_msg, 1, 0
    CPY16 last_nmi_count, itoa_in
    jsr itoa
    jsr pad_itoa_out
    STO16 itoa_out, hd44780_string
    jsr hd44780_write_string
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
