    .ifndef seen_6552
seen_6552 .equ 1

; Constant definitions for the 6552

W6552_BASE      = $6000
W6552_REG_ORB   = W6552_BASE + 0
W6552_REG_ORA   = W6552_BASE + 1
W6552_REG_DDRB  = W6552_BASE + 2
W6552_REG_DDRA  = W6552_BASE + 3
W6552_REG_T1CL  = W6552_BASE + 4
W6552_REG_T1CH  = W6552_BASE + 5
W6552_REG_T1LL  = W6552_BASE + 6
W6552_REG_T1LH  = W6552_BASE + 7
W6552_REG_T2CL  = W6552_BASE + 8
W6552_REG_T2CH  = W6552_BASE + 9
W6552_REG_SR    = W6552_BASE + 10
W6552_REG_ACR   = W6552_BASE + 11
W6552_REG_PCR   = W6552_BASE + 12
W6552_REG_IFR   = W6552_BASE + 13
W6552_REG_IER   = W6552_BASE + 14
W6552_REG_ORANH = W6552_BASE + 15

W6552_REG_IRB   = W6552_REG_ORB
W6552_REG_IRA   = W6552_REG_ORA
W6552_REG_IRANH = W6552_REG_ORANH

W6552_T1_MODE_MASK              = %11000000
W6552_T1_MODE_SINGLE_SHOT       = %00000000
W6552_T1_MODE_CONTINUOUS        = %01000000
W6552_T1_MODE_SINGLE_SHOT_PB6   = %10000000
W6552_T1_MODE_CONTINUOUS_PB6    = %11000000

W6552_T2_MODE_MASK              = %00100000
W6552_T2_MODE_SINGLE_SHOT       = %00000000
W6552_T2_MODE_COUNTDOWN_PB6     = %00100000

W6552_IER_ENABLE                = %10000000
W6552_IER_DISABLE               = %00000000
W6552_IER_CA2                   = %00000001
W6552_IER_CA1                   = %00000010
W6552_IER_SR                    = %00000100
W6552_IER_CB2                   = %00001000
W6552_IER_CB1                   = %00010000
W6552_IER_T2                    = %00100000
W6552_IER_T1                    = %01000000
W6552_IER_IRQ                   = %10000000

W6552_WRITE .macro register, value
    lda #\value
    sta \register
    .endm

W6552_READ .macro register
    lda \register
    .endm

W6552_WRITE_MASKED .macro register, value, mask
    lda \register
    and #(\mask ^ $ff)
    ora #\value
    sta \register
    .endm

W6552_SET_T1_MODE .macro mode
    W6552_WRITE_MASKED W6552_REG_ACR, \mode, W6552_T1_MODE_MASK
    .endm

W6552_SET_T1_COUNTER .macro value
    W6552_WRITE W6552_REG_T1CL, <\value
    W6552_WRITE W6552_REG_T1CH, >\value
    .endm

W6552_SET_T2_MODE .macro mode
    W6552_WRITE_MASKED W6552_REG_ACR, \mode, W6552_T2_MODE_MASK
    .endm

W6552_SET_T2_COUNTER .macro value
    W6552_WRITE W6552_REG_T2CL, <\value
    W6552_WRITE W6552_REG_T2CH, >\value
    .endm

W6552_ENABLE_INTERRUPTS .macro mask
    W6552_WRITE W6552_REG_IER, (\mask | W6552_IER_ENABLE)
    .endm

W6552_DISABLE_INTERRUPTS .macro mask
    W6552_WRITE W6552_REG_IER, (\mask | W6552_IER_DISABLE)
    .endm

; Reading T1C-L clears the T1 interrupt flag
W6552_ACK_T1 .macro
    W6552_READ W6552_REG_T1CL
    .endm

    .endif ; seen_6552
