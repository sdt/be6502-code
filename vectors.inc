;------------------------------------------------------------------------------
; vectors.inc
;
; Intended to be included at the very end of top-level program files.
; Sets up interrupt handlers.
;
; Expects the reset label to be defined.
; The irq and nmi labels are optional. If defined, they will be used. If missing
; they will default to a trivial handler that just does rti.

    .ifndef seen_vectors
seen_vectors .equ 1

; Default interrupt handler just returns
default_interrupt_handler:
    rti

    .ifndef nmi
nmi = default_interrupt_handler
    .endif

    .ifndef irq
irq = default_interrupt_handler
    .endif

; Interrupt vector table.
    .org $fffa

nmi_vector:     ; $fffa
    .word nmi

reset_vector:   ; $fffc
    .word reset

irq_vector:     ; $fffe
    .word irq

    .endif ; seen_vectors
