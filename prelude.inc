;------------------------------------------------------------------------------
; prelude.inc
;
; Intended to be included at the very start of top-level program files.
; Sets up zero page, bss, stack, and code segments.
    .ifndef seen_prelude
seen_prelude .equ 1

;------------------------------------------------------------------------------
; Zero page variables
;
; Define a byte in the zero page:
;   ZPB symbol
;
; Define a word in the zero page:
;   ZPW symbol
;
; Define N bytes in the zero page:
;   ZPDATA symbol, N
;
; We keep track of the current offset in the zero page with _ZEROPAGE_OFFSET,
; and abort if we overflow past the zero page.
_zeropage_offset .set $0000

ZPDATA .macro symbol, bytes
\symbol = _zeropage_offset
_zeropage_offset .set _zeropage_offset + \bytes
    .assert _zeropage_offset <= 256, "Zero page overflow"
    .endm

ZPB .macro symbol
    ZPDATA \symbol, 1
    .endm

ZPW .macro symbol
    ZPDATA \symbol, 2
    .endm

STORE .macro dest, value
    .ifeq \value
    stz \dest
    .else
    lda #\value
    stz \dest
    .endif
    .endm

;------------------------------------------------------------------------------
; RAM variables. These start at $0200, and are wrapped in .dsect / .dend
; directives
    .dsect
    .org $0200
    .dend

;------------------------------------------------------------------------------
; ROM section. Starts at $8000
    .org $8000

; Set up the stack, and then jump to the start label.
reset:
    ldx #$ff
    txs
    jmp start

    .endif ; seen_prelude
