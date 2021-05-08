    .ifndef seen_6522
seen_6522 .equ 1

; Constant definitions for the 6522

W6522_BASE      = $6000
W6522_REG_ORB   = W6522_BASE + 0
W6522_REG_ORA   = W6522_BASE + 1
W6522_REG_DDRB  = W6522_BASE + 2
W6522_REG_DDRA  = W6522_BASE + 3
W6522_REG_T1CL  = W6522_BASE + 4
W6522_REG_T1CH  = W6522_BASE + 5
W6522_REG_T1LL  = W6522_BASE + 6
W6522_REG_T1LH  = W6522_BASE + 7
W6522_REG_T2CL  = W6522_BASE + 8
W6522_REG_T2CH  = W6522_BASE + 9
W6522_REG_SR    = W6522_BASE + 10
W6522_REG_ACR   = W6522_BASE + 11
W6522_REG_PCR   = W6522_BASE + 12
W6522_REG_IFR   = W6522_BASE + 13
W6522_REG_IER   = W6522_BASE + 14
W6522_REG_ORANH = W6522_BASE + 15

W6522_REG_IRB   = W6522_REG_ORB
W6522_REG_IRA   = W6522_REG_ORA
W6522_REG_IRANH = W6522_REG_ORANH

W6522_T1_MODE_MASK              = %11000000
W6522_T1_MODE_SINGLE_SHOT       = %00000000
W6522_T1_MODE_CONTINUOUS        = %01000000
W6522_T1_MODE_SINGLE_SHOT_PB6   = %10000000
W6522_T1_MODE_CONTINUOUS_PB6    = %11000000

W6522_T2_MODE_MASK              = %00100000
W6522_T2_MODE_SINGLE_SHOT       = %00000000
W6522_T2_MODE_COUNTDOWN_PB6     = %00100000

W6522_IER_ENABLE                = %10000000
W6522_IER_DISABLE               = %00000000
W6522_IER_CA2                   = %00000001
W6522_IER_CA1                   = %00000010
W6522_IER_SR                    = %00000100
W6522_IER_CB2                   = %00001000
W6522_IER_CB1                   = %00010000
W6522_IER_T2                    = %00100000
W6522_IER_T1                    = %01000000
W6522_IER_IRQ                   = %10000000
W6522_IER_ALL                   = %01111111

W6552_IFR_BIT_CA2               = 0
W6552_IFR_BIT_CA1               = 1
W6552_IFR_BIT_SR                = 2
W6552_IFR_BIT_CB2               = 3
W6552_IFR_BIT_CB1               = 4
W6552_IFR_BIT_T2                = 5
W6552_IFR_BIT_T1                = 6
W6552_IFR_BIT_ANY               = 7

W6522_SR_MODE_MASK              = %00011100
W6522_SR_MODE_DISABLED          = %00000000
W6522_SR_MODE_IN_T2             = %00000100
W6522_SR_MODE_IN_PHI2           = %00001000
W6522_SR_MODE_IN_CB1            = %00001100
W6522_SR_MODE_OUT_FREE          = %00010000
W6522_SR_MODE_OUT_T2            = %00010100
W6522_SR_MODE_OUT_PHI2          = %00011000
W6522_SR_MODE_OUT_CB1           = %00011100

W6522_WRITE .macro register, value
        .ifeq \value
    stz \register
        .else
    lda #\value
    sta \register
        .endif
    .endm

W6522_READ .macro register
    lda \register
    .endm

W6522_WRITE_MASKED .macro register, value, mask
    lda \register
    and #(\mask ^ $ff)
    ora #\value
    sta \register
    .endm

W6522_SET_T1_MODE .macro mode
    W6522_WRITE_MASKED W6522_REG_ACR, \mode, W6522_T1_MODE_MASK
    .endm

W6522_SET_T1_COUNTER .macro value
    W6522_WRITE W6522_REG_T1CL, <\value
    W6522_WRITE W6522_REG_T1CH, >\value
    .endm

W6522_SET_T2_MODE .macro mode
    W6522_WRITE_MASKED W6522_REG_ACR, \mode, W6522_T2_MODE_MASK
    .endm

W6522_SET_T2_COUNTER .macro value
    W6522_WRITE W6522_REG_T2CL, <\value
    W6522_WRITE W6522_REG_T2CH, >\value
    .endm

W6522_SET_SR_MODE .macro mode
    W6522_WRITE_MASKED W6522_REG_ACR, \mode, W6522_SR_MODE_MASK
    .endm

W6522_ENABLE_INTERRUPTS .macro mask
    W6522_WRITE W6522_REG_IER, (\mask | W6522_IER_ENABLE)
    .endm

W6522_DISABLE_INTERRUPTS .macro mask
    W6522_WRITE W6522_REG_IER, (\mask | W6522_IER_DISABLE)
    .endm

; Reading T1C-L clears the T1 interrupt flag
W6522_ACK_T1 .macro
    W6522_READ W6522_REG_T1CL
    .endm

    .endif ; seen_6522
