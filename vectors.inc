; Interrupt vector table.
    .org $fffa

nmi_vector:
    .ifndef nmi
nmi = $adde
    .endif
    .word nmi

reset_vector:
    .word reset

irq_vector:
    .ifndef irq
irq = $adde
    .endif
    .word irq
