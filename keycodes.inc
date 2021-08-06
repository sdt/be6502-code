    .ifndef seen_keycodes
seen_keycodes .equ 1

KEY_NONE             = $00 ; No key
KEY_A                = $04 ; a A
KEY_B                = $05 ; b B
KEY_C                = $06 ; c C
KEY_D                = $07 ; d D
KEY_E                = $08 ; e E
KEY_F                = $09 ; f F
KEY_G                = $0a ; g G
KEY_H                = $0b ; h H
KEY_I                = $0c ; i I
KEY_J                = $0d ; j J
KEY_K                = $0e ; k K
KEY_L                = $0f ; l L
KEY_M                = $10 ; m M
KEY_N                = $11 ; n N
KEY_O                = $12 ; o O
KEY_P                = $13 ; p P
KEY_Q                = $14 ; q Q
KEY_R                = $15 ; r R
KEY_S                = $16 ; s S
KEY_T                = $17 ; t T
KEY_U                = $18 ; u U
KEY_V                = $19 ; v V
KEY_W                = $1a ; w W
KEY_X                = $1b ; x X
KEY_Y                = $1c ; y Y
KEY_Z                = $1d ; z Z
KEY_1                = $1e ; 1 !
KEY_2                = $1f ; 2 @
KEY_3                = $20 ; 3 #
KEY_4                = $21 ; 4 $
KEY_5                = $22 ; 5 %
KEY_6                = $23 ; 6 ^
KEY_7                = $24 ; 7 &
KEY_8                = $25 ; 8 *
KEY_9                = $26 ; 9 (
KEY_0                = $27 ; 0 )
KEY_ENTER            = $28 ; Return
KEY_ESCAPE           = $29 ; Escape
KEY_BACKSPACE        = $2a ; Backspace
KEY_TAB              = $2b ; Tab
KEY_SPACE            = $2c ; Space
KEY_MINUS            = $2d ; - _
KEY_EQUAL            = $2e ; = +
KEY_LEFTBRACE        = $2f ; [ {
KEY_RIGHTBRACE       = $30 ; ] }
KEY_BACKSLASH        = $31 ; \ |
KEY_HASHTILDE        = $32 ; Europe 1
KEY_SEMICOLON        = $33 ; ; :
KEY_APOSTROPHE       = $34 ; ' "
KEY_GRAVE            = $35 ; ` ~
KEY_COMMA            = $36 ; , <
KEY_DOT              = $37 ; . >
KEY_SLASH            = $38 ; / ?
KEY_CAPSLOCK         = $39 ; Caps Lock
KEY_F1               = $3a ; F1
KEY_F2               = $3b ; F2
KEY_F3               = $3c ; F3
KEY_F4               = $3d ; F4
KEY_F5               = $3e ; F5
KEY_F6               = $3f ; F6
KEY_F7               = $40 ; F7
KEY_F8               = $41 ; F8
KEY_F9               = $42 ; F9
KEY_F10              = $43 ; F10
KEY_F11              = $44 ; F11
KEY_F12              = $45 ; F12
KEY_SYSRQ            = $46 ; Print Screen
KEY_SCROLLLOCK       = $47 ; Scroll Lock
KEY_BREAK            = $48 ; Break (Ctrl-Pause)
KEY_PAUSE            = $48 ; Pause
KEY_INSERT           = $49 ; Insert
KEY_HOME             = $4a ; Home
KEY_PAGEUP           = $4b ; Page Up
KEY_DELETE           = $4c ; Delete
KEY_END              = $4d ; End
KEY_PAGEDOWN         = $4e ; Page Down
KEY_RIGHT            = $4f ; Right Arrow
KEY_LEFT             = $50 ; Left Arrow
KEY_DOWN             = $51 ; Down Arrow
KEY_UP               = $52 ; Up Arrow
KEY_NUMLOCK          = $53 ; Num Lock
KEY_KPSLASH          = $54 ; Keypad /
KEY_KPASTERISK       = $55 ; Keypad *
KEY_KPMINUS          = $56 ; Keypad -
KEY_KPPLUS           = $57 ; Keypad +
KEY_KPENTER          = $58 ; Keypad Enter
KEY_KP1              = $59 ; Keypad 1 End
KEY_KP2              = $5a ; Keypad 2 Down
KEY_KP3              = $5b ; Keypad 3 PageDn
KEY_KP4              = $5c ; Keypad 4 Left
KEY_KP5              = $5d ; Keypad 5
KEY_KP6              = $5e ; Keypad 6 Right
KEY_KP7              = $5f ; Keypad 7 Home
KEY_KP8              = $60 ; Keypad 8 Up
KEY_KP9              = $61 ; Keypad 9 PageUp
KEY_KP0              = $62 ; Keypad 0 Insert
KEY_KPDOT            = $63 ; Keypad . Delete
KEY_102ND            = $64 ; Europe 2
KEY_MENU             = $65 ; Menu
KEY_POWER            = $66 ; Keyboard Power
KEY_KPEQUAL          = $67 ; Keypad =
KEY_LEFTCTRL         = $68 ; Left Control
KEY_LEFTSHIFT        = $69 ; Left Shift
KEY_LEFTALT          = $6a ; Left Alt
KEY_LEFTWIN          = $6b ; Left Windows
KEY_RIGHTCTRL        = $6c ; Right Control
KEY_RIGHTSHIFT       = $6d ; Right Shift
KEY_RIGHTALT         = $6e ; Right Alt
KEY_RIGHTWIN         = $6f ; Right Windows

ps2kbd_standard_keycode:
    .db 0                    ; $00
    .db KEY_F9               ; $01
    .db 0                    ; $02
    .db KEY_F5               ; $03
    .db KEY_F3               ; $04
    .db KEY_F1               ; $05
    .db KEY_F2               ; $06
    .db KEY_F12              ; $07
    .db 0                    ; $08
    .db KEY_F10              ; $09
    .db KEY_F8               ; $0a
    .db KEY_F6               ; $0b
    .db KEY_F4               ; $0c
    .db KEY_TAB              ; $0d
    .db KEY_GRAVE            ; $0e
    .db KEY_KPEQUAL          ; $0f
    .db 0                    ; $10
    .db KEY_LEFTALT          ; $11
    .db KEY_LEFTSHIFT        ; $12
    .db 0                    ; $13
    .db KEY_LEFTCTRL         ; $14
    .db KEY_Q                ; $15
    .db KEY_1                ; $16
    .db 0                    ; $17
    .db 0                    ; $18
    .db 0                    ; $19
    .db KEY_Z                ; $1a
    .db KEY_S                ; $1b
    .db KEY_A                ; $1c
    .db KEY_W                ; $1d
    .db KEY_2                ; $1e
    .db 0                    ; $1f
    .db 0                    ; $20
    .db KEY_C                ; $21
    .db KEY_X                ; $22
    .db KEY_D                ; $23
    .db KEY_E                ; $24
    .db KEY_4                ; $25
    .db KEY_3                ; $26
    .db 0                    ; $27
    .db 0                    ; $28
    .db KEY_SPACE            ; $29
    .db KEY_V                ; $2a
    .db KEY_F                ; $2b
    .db KEY_T                ; $2c
    .db KEY_R                ; $2d
    .db KEY_5                ; $2e
    .db 0                    ; $2f
    .db 0                    ; $30
    .db KEY_N                ; $31
    .db KEY_B                ; $32
    .db KEY_H                ; $33
    .db KEY_G                ; $34
    .db KEY_Y                ; $35
    .db KEY_6                ; $36
    .db 0                    ; $37
    .db 0                    ; $38
    .db 0                    ; $39
    .db KEY_M                ; $3a
    .db KEY_J                ; $3b
    .db KEY_U                ; $3c
    .db KEY_7                ; $3d
    .db KEY_8                ; $3e
    .db 0                    ; $3f
    .db 0                    ; $40
    .db KEY_COMMA            ; $41
    .db KEY_K                ; $42
    .db KEY_I                ; $43
    .db KEY_O                ; $44
    .db KEY_0                ; $45
    .db KEY_9                ; $46
    .db 0                    ; $47
    .db 0                    ; $48
    .db KEY_DOT              ; $49
    .db KEY_SLASH            ; $4a
    .db KEY_L                ; $4b
    .db KEY_SEMICOLON        ; $4c
    .db KEY_P                ; $4d
    .db KEY_MINUS            ; $4e
    .db 0                    ; $4f
    .db 0                    ; $50
    .db 0                    ; $51
    .db KEY_APOSTROPHE       ; $52
    .db 0                    ; $53
    .db KEY_LEFTBRACE        ; $54
    .db KEY_EQUAL            ; $55
    .db 0                    ; $56
    .db 0                    ; $57
    .db KEY_CAPSLOCK         ; $58
    .db KEY_RIGHTSHIFT       ; $59
    .db KEY_ENTER            ; $5a
    .db KEY_RIGHTBRACE       ; $5b
    .db 0                    ; $5c
    .db KEY_HASHTILDE        ; $5d
    .db 0                    ; $5e
    .db 0                    ; $5f
    .db 0                    ; $60
    .db KEY_102ND            ; $61
    .db 0                    ; $62
    .db 0                    ; $63
    .db 0                    ; $64
    .db 0                    ; $65
    .db KEY_BACKSPACE        ; $66
    .db 0                    ; $67
    .db 0                    ; $68
    .db KEY_KP1              ; $69
    .db 0                    ; $6a
    .db KEY_KP4              ; $6b
    .db KEY_KP7              ; $6c
    .db 0                    ; $6d
    .db 0                    ; $6e
    .db 0                    ; $6f
    .db KEY_KP0              ; $70
    .db KEY_KPDOT            ; $71
    .db KEY_KP2              ; $72
    .db KEY_KP5              ; $73
    .db KEY_KP6              ; $74
    .db KEY_KP8              ; $75
    .db KEY_ESCAPE           ; $76
    .db KEY_NUMLOCK          ; $77
    .db KEY_F11              ; $78
    .db KEY_KPPLUS           ; $79
    .db KEY_KP3              ; $7a
    .db KEY_KPMINUS          ; $7b
    .db KEY_KPASTERISK       ; $7c
    .db KEY_KP9              ; $7d
    .db KEY_SCROLLLOCK       ; $7e
    .db 0                    ; $7f
    .db 0                    ; $80
    .db 0                    ; $81
    .db 0                    ; $82
    .db KEY_F7               ; $83
    .db 0                    ; $84
    .db 0                    ; $85
    .db 0                    ; $86
    .db 0                    ; $87
    .db 0                    ; $88
    .db 0                    ; $89
    .db 0                    ; $8a
    .db 0                    ; $8b
    .db 0                    ; $8c
    .db 0                    ; $8d
    .db 0                    ; $8e
    .db 0                    ; $8f
    .db 0                    ; $90
    .db 0                    ; $91
    .db 0                    ; $92
    .db 0                    ; $93
    .db 0                    ; $94
    .db 0                    ; $95
    .db 0                    ; $96
    .db 0                    ; $97
    .db 0                    ; $98
    .db 0                    ; $99
    .db 0                    ; $9a
    .db 0                    ; $9b
    .db 0                    ; $9c
    .db 0                    ; $9d
    .db 0                    ; $9e
    .db 0                    ; $9f
    .db 0                    ; $a0
    .db 0                    ; $a1
    .db 0                    ; $a2
    .db 0                    ; $a3
    .db 0                    ; $a4
    .db 0                    ; $a5
    .db 0                    ; $a6
    .db 0                    ; $a7
    .db 0                    ; $a8
    .db 0                    ; $a9
    .db 0                    ; $aa
    .db 0                    ; $ab
    .db 0                    ; $ac
    .db 0                    ; $ad
    .db 0                    ; $ae
    .db 0                    ; $af
    .db 0                    ; $b0
    .db 0                    ; $b1
    .db 0                    ; $b2
    .db 0                    ; $b3
    .db 0                    ; $b4
    .db 0                    ; $b5
    .db 0                    ; $b6
    .db 0                    ; $b7
    .db 0                    ; $b8
    .db 0                    ; $b9
    .db 0                    ; $ba
    .db 0                    ; $bb
    .db 0                    ; $bc
    .db 0                    ; $bd
    .db 0                    ; $be
    .db 0                    ; $bf
    .db 0                    ; $c0
    .db 0                    ; $c1
    .db 0                    ; $c2
    .db 0                    ; $c3
    .db 0                    ; $c4
    .db 0                    ; $c5
    .db 0                    ; $c6
    .db 0                    ; $c7
    .db 0                    ; $c8
    .db 0                    ; $c9
    .db 0                    ; $ca
    .db 0                    ; $cb
    .db 0                    ; $cc
    .db 0                    ; $cd
    .db 0                    ; $ce
    .db 0                    ; $cf
    .db 0                    ; $d0
    .db 0                    ; $d1
    .db 0                    ; $d2
    .db 0                    ; $d3
    .db 0                    ; $d4
    .db 0                    ; $d5
    .db 0                    ; $d6
    .db 0                    ; $d7
    .db 0                    ; $d8
    .db 0                    ; $d9
    .db 0                    ; $da
    .db 0                    ; $db
    .db 0                    ; $dc
    .db 0                    ; $dd
    .db 0                    ; $de
    .db 0                    ; $df
    .db 0                    ; $e0
    .db 0                    ; $e1
    .db 0                    ; $e2
    .db 0                    ; $e3
    .db 0                    ; $e4
    .db 0                    ; $e5
    .db 0                    ; $e6
    .db 0                    ; $e7
    .db 0                    ; $e8
    .db 0                    ; $e9
    .db 0                    ; $ea
    .db 0                    ; $eb
    .db 0                    ; $ec
    .db 0                    ; $ed
    .db 0                    ; $ee
    .db 0                    ; $ef
    .db 0                    ; $f0
    .db 0                    ; $f1
    .db 0                    ; $f2
    .db 0                    ; $f3
    .db 0                    ; $f4
    .db 0                    ; $f5
    .db 0                    ; $f6
    .db 0                    ; $f7
    .db 0                    ; $f8
    .db 0                    ; $f9
    .db 0                    ; $fa
    .db 0                    ; $fb
    .db 0                    ; $fc
    .db 0                    ; $fd
    .db 0                    ; $fe
    .db 0                    ; $ff

ps2kbd_extended_keycode:
    .db 0                    ; $00
    .db 0                    ; $01
    .db 0                    ; $02
    .db 0                    ; $03
    .db 0                    ; $04
    .db 0                    ; $05
    .db 0                    ; $06
    .db 0                    ; $07
    .db 0                    ; $08
    .db 0                    ; $09
    .db 0                    ; $0a
    .db 0                    ; $0b
    .db 0                    ; $0c
    .db 0                    ; $0d
    .db 0                    ; $0e
    .db 0                    ; $0f
    .db 0                    ; $10
    .db KEY_RIGHTALT         ; $11
    .db 0                    ; $12
    .db 0                    ; $13
    .db KEY_RIGHTCTRL        ; $14
    .db 0                    ; $15
    .db 0                    ; $16
    .db 0                    ; $17
    .db 0                    ; $18
    .db 0                    ; $19
    .db 0                    ; $1a
    .db 0                    ; $1b
    .db 0                    ; $1c
    .db 0                    ; $1d
    .db 0                    ; $1e
    .db KEY_LEFTWIN          ; $1f
    .db 0                    ; $20
    .db 0                    ; $21
    .db 0                    ; $22
    .db 0                    ; $23
    .db 0                    ; $24
    .db 0                    ; $25
    .db 0                    ; $26
    .db KEY_RIGHTWIN         ; $27
    .db 0                    ; $28
    .db 0                    ; $29
    .db 0                    ; $2a
    .db 0                    ; $2b
    .db 0                    ; $2c
    .db 0                    ; $2d
    .db 0                    ; $2e
    .db KEY_MENU             ; $2f
    .db 0                    ; $30
    .db 0                    ; $31
    .db 0                    ; $32
    .db 0                    ; $33
    .db 0                    ; $34
    .db 0                    ; $35
    .db 0                    ; $36
    .db KEY_POWER            ; $37
    .db 0                    ; $38
    .db 0                    ; $39
    .db 0                    ; $3a
    .db 0                    ; $3b
    .db 0                    ; $3c
    .db 0                    ; $3d
    .db 0                    ; $3e
    .db 0                    ; $3f
    .db 0                    ; $40
    .db 0                    ; $41
    .db 0                    ; $42
    .db 0                    ; $43
    .db 0                    ; $44
    .db 0                    ; $45
    .db 0                    ; $46
    .db 0                    ; $47
    .db 0                    ; $48
    .db 0                    ; $49
    .db KEY_KPSLASH          ; $4a
    .db 0                    ; $4b
    .db 0                    ; $4c
    .db 0                    ; $4d
    .db 0                    ; $4e
    .db 0                    ; $4f
    .db 0                    ; $50
    .db 0                    ; $51
    .db 0                    ; $52
    .db 0                    ; $53
    .db 0                    ; $54
    .db 0                    ; $55
    .db 0                    ; $56
    .db 0                    ; $57
    .db 0                    ; $58
    .db 0                    ; $59
    .db KEY_KPENTER          ; $5a
    .db 0                    ; $5b
    .db 0                    ; $5c
    .db 0                    ; $5d
    .db 0                    ; $5e
    .db 0                    ; $5f
    .db 0                    ; $60
    .db 0                    ; $61
    .db 0                    ; $62
    .db 0                    ; $63
    .db 0                    ; $64
    .db 0                    ; $65
    .db 0                    ; $66
    .db 0                    ; $67
    .db 0                    ; $68
    .db KEY_END              ; $69
    .db 0                    ; $6a
    .db KEY_LEFT             ; $6b
    .db KEY_HOME             ; $6c
    .db 0                    ; $6d
    .db 0                    ; $6e
    .db 0                    ; $6f
    .db KEY_INSERT           ; $70
    .db KEY_DELETE           ; $71
    .db KEY_DOWN             ; $72
    .db 0                    ; $73
    .db KEY_RIGHT            ; $74
    .db KEY_UP               ; $75
    .db 0                    ; $76
    .db 0                    ; $77
    .db 0                    ; $78
    .db 0                    ; $79
    .db KEY_PAGEDOWN         ; $7a
    .db 0                    ; $7b
    .db KEY_SYSRQ            ; $7c
    .db KEY_PAGEUP           ; $7d
    .db KEY_BREAK            ; $7e
    .db 0                    ; $7f
    .db 0                    ; $80
    .db 0                    ; $81
    .db 0                    ; $82
    .db 0                    ; $83
    .db 0                    ; $84
    .db 0                    ; $85
    .db 0                    ; $86
    .db 0                    ; $87
    .db 0                    ; $88
    .db 0                    ; $89
    .db 0                    ; $8a
    .db 0                    ; $8b
    .db 0                    ; $8c
    .db 0                    ; $8d
    .db 0                    ; $8e
    .db 0                    ; $8f
    .db 0                    ; $90
    .db 0                    ; $91
    .db 0                    ; $92
    .db 0                    ; $93
    .db 0                    ; $94
    .db 0                    ; $95
    .db 0                    ; $96
    .db 0                    ; $97
    .db 0                    ; $98
    .db 0                    ; $99
    .db 0                    ; $9a
    .db 0                    ; $9b
    .db 0                    ; $9c
    .db 0                    ; $9d
    .db 0                    ; $9e
    .db 0                    ; $9f
    .db 0                    ; $a0
    .db 0                    ; $a1
    .db 0                    ; $a2
    .db 0                    ; $a3
    .db 0                    ; $a4
    .db 0                    ; $a5
    .db 0                    ; $a6
    .db 0                    ; $a7
    .db 0                    ; $a8
    .db 0                    ; $a9
    .db 0                    ; $aa
    .db 0                    ; $ab
    .db 0                    ; $ac
    .db 0                    ; $ad
    .db 0                    ; $ae
    .db 0                    ; $af
    .db 0                    ; $b0
    .db 0                    ; $b1
    .db 0                    ; $b2
    .db 0                    ; $b3
    .db 0                    ; $b4
    .db 0                    ; $b5
    .db 0                    ; $b6
    .db 0                    ; $b7
    .db 0                    ; $b8
    .db 0                    ; $b9
    .db 0                    ; $ba
    .db 0                    ; $bb
    .db 0                    ; $bc
    .db 0                    ; $bd
    .db 0                    ; $be
    .db 0                    ; $bf
    .db 0                    ; $c0
    .db 0                    ; $c1
    .db 0                    ; $c2
    .db 0                    ; $c3
    .db 0                    ; $c4
    .db 0                    ; $c5
    .db 0                    ; $c6
    .db 0                    ; $c7
    .db 0                    ; $c8
    .db 0                    ; $c9
    .db 0                    ; $ca
    .db 0                    ; $cb
    .db 0                    ; $cc
    .db 0                    ; $cd
    .db 0                    ; $ce
    .db 0                    ; $cf
    .db 0                    ; $d0
    .db 0                    ; $d1
    .db 0                    ; $d2
    .db 0                    ; $d3
    .db 0                    ; $d4
    .db 0                    ; $d5
    .db 0                    ; $d6
    .db 0                    ; $d7
    .db 0                    ; $d8
    .db 0                    ; $d9
    .db 0                    ; $da
    .db 0                    ; $db
    .db 0                    ; $dc
    .db 0                    ; $dd
    .db 0                    ; $de
    .db 0                    ; $df
    .db 0                    ; $e0
    .db 0                    ; $e1
    .db 0                    ; $e2
    .db 0                    ; $e3
    .db 0                    ; $e4
    .db 0                    ; $e5
    .db 0                    ; $e6
    .db 0                    ; $e7
    .db 0                    ; $e8
    .db 0                    ; $e9
    .db 0                    ; $ea
    .db 0                    ; $eb
    .db 0                    ; $ec
    .db 0                    ; $ed
    .db 0                    ; $ee
    .db 0                    ; $ef
    .db 0                    ; $f0
    .db 0                    ; $f1
    .db 0                    ; $f2
    .db 0                    ; $f3
    .db 0                    ; $f4
    .db 0                    ; $f5
    .db 0                    ; $f6
    .db 0                    ; $f7
    .db 0                    ; $f8
    .db 0                    ; $f9
    .db 0                    ; $fa
    .db 0                    ; $fb
    .db 0                    ; $fc
    .db 0                    ; $fd
    .db 0                    ; $fe
    .db 0                    ; $ff

ps2kbd_plain_ascii:
    .db    0   ; $00
    .db    0   ; $01
    .db    0   ; $02
    .db    0   ; $03
    .db    $61 ; $04 KEY_A
    .db    $62 ; $05 KEY_B
    .db    $63 ; $06 KEY_C
    .db    $64 ; $07 KEY_D
    .db    $65 ; $08 KEY_E
    .db    $66 ; $09 KEY_F
    .db    $67 ; $0a KEY_G
    .db    $68 ; $0b KEY_H
    .db    $69 ; $0c KEY_I
    .db    $6a ; $0d KEY_J
    .db    $6b ; $0e KEY_K
    .db    $6c ; $0f KEY_L
    .db    $6d ; $10 KEY_M
    .db    $6e ; $11 KEY_N
    .db    $6f ; $12 KEY_O
    .db    $70 ; $13 KEY_P
    .db    $71 ; $14 KEY_Q
    .db    $72 ; $15 KEY_R
    .db    $73 ; $16 KEY_S
    .db    $74 ; $17 KEY_T
    .db    $75 ; $18 KEY_U
    .db    $76 ; $19 KEY_V
    .db    $77 ; $1a KEY_W
    .db    $78 ; $1b KEY_X
    .db    $79 ; $1c KEY_Y
    .db    $7a ; $1d KEY_Z
    .db    $31 ; $1e KEY_1
    .db    $32 ; $1f KEY_2
    .db    $33 ; $20 KEY_3
    .db    $34 ; $21 KEY_4
    .db    $35 ; $22 KEY_5
    .db    $36 ; $23 KEY_6
    .db    $37 ; $24 KEY_7
    .db    $38 ; $25 KEY_8
    .db    $39 ; $26 KEY_9
    .db    $30 ; $27 KEY_0
    .db    $0a ; $28 KEY_ENTER
    .db    $1b ; $29 KEY_ESCAPE
    .db    $08 ; $2a KEY_BACKSPACE
    .db    $09 ; $2b KEY_TAB
    .db    $20 ; $2c KEY_SPACE
    .db    $2d ; $2d KEY_MINUS
    .db    $3d ; $2e KEY_EQUAL
    .db    $5b ; $2f KEY_LEFTBRACE
    .db    $5d ; $30 KEY_RIGHTBRACE
    .db    $5c ; $31 KEY_BACKSLASH
    .db    0   ; $32
    .db    $3b ; $33 KEY_SEMICOLON
    .db    $27 ; $34 KEY_APOSTROPHE
    .db    $60 ; $35 KEY_GRAVE
    .db    $2c ; $36 KEY_COMMA
    .db    $2e ; $37 KEY_DOT
    .db    $2f ; $38 KEY_SLASH
    .db    0   ; $39
    .db    0   ; $3a
    .db    0   ; $3b
    .db    0   ; $3c
    .db    0   ; $3d
    .db    0   ; $3e
    .db    0   ; $3f
    .db    0   ; $40
    .db    0   ; $41
    .db    0   ; $42
    .db    0   ; $43
    .db    0   ; $44
    .db    0   ; $45
    .db    0   ; $46
    .db    0   ; $47
    .db    0   ; $48
    .db    0   ; $49
    .db    0   ; $4a
    .db    0   ; $4b
    .db    0   ; $4c
    .db    0   ; $4d
    .db    0   ; $4e
    .db    0   ; $4f
    .db    0   ; $50
    .db    0   ; $51
    .db    0   ; $52
    .db    0   ; $53
    .db    $2f ; $54 KEY_KPSLASH
    .db    $2a ; $55 KEY_KPASTERISK
    .db    $2d ; $56 KEY_KPMINUS
    .db    $2b ; $57 KEY_KPPLUS
    .db    $0a ; $58 KEY_KPENTER
    .db    $31 ; $59 KEY_KP1
    .db    $32 ; $5a KEY_KP2
    .db    $33 ; $5b KEY_KP3
    .db    $34 ; $5c KEY_KP4
    .db    $35 ; $5d KEY_KP5
    .db    $36 ; $5e KEY_KP6
    .db    $37 ; $5f KEY_KP7
    .db    $38 ; $60 KEY_KP8
    .db    $39 ; $61 KEY_KP9
    .db    $30 ; $62 KEY_KP0
    .db    0   ; $63
    .db    0   ; $64
    .db    0   ; $65
    .db    0   ; $66
    .db    $3d ; $67 KEY_KPEQUAL
    .db    0   ; $68
    .db    0   ; $69
    .db    0   ; $6a
    .db    0   ; $6b
    .db    0   ; $6c
    .db    0   ; $6d
    .db    0   ; $6e
    .db    0   ; $6f
    .db    0   ; $70
    .db    0   ; $71
    .db    0   ; $72
    .db    0   ; $73
    .db    0   ; $74
    .db    0   ; $75
    .db    0   ; $76
    .db    0   ; $77
    .db    0   ; $78
    .db    0   ; $79
    .db    0   ; $7a
    .db    0   ; $7b
    .db    0   ; $7c
    .db    0   ; $7d
    .db    0   ; $7e
    .db    0   ; $7f
    .db    0   ; $80
    .db    0   ; $81
    .db    0   ; $82
    .db    0   ; $83
    .db    0   ; $84
    .db    0   ; $85
    .db    0   ; $86
    .db    0   ; $87
    .db    0   ; $88
    .db    0   ; $89
    .db    0   ; $8a
    .db    0   ; $8b
    .db    0   ; $8c
    .db    0   ; $8d
    .db    0   ; $8e
    .db    0   ; $8f
    .db    0   ; $90
    .db    0   ; $91
    .db    0   ; $92
    .db    0   ; $93
    .db    0   ; $94
    .db    0   ; $95
    .db    0   ; $96
    .db    0   ; $97
    .db    0   ; $98
    .db    0   ; $99
    .db    0   ; $9a
    .db    0   ; $9b
    .db    0   ; $9c
    .db    0   ; $9d
    .db    0   ; $9e
    .db    0   ; $9f
    .db    0   ; $a0
    .db    0   ; $a1
    .db    0   ; $a2
    .db    0   ; $a3
    .db    0   ; $a4
    .db    0   ; $a5
    .db    0   ; $a6
    .db    0   ; $a7
    .db    0   ; $a8
    .db    0   ; $a9
    .db    0   ; $aa
    .db    0   ; $ab
    .db    0   ; $ac
    .db    0   ; $ad
    .db    0   ; $ae
    .db    0   ; $af
    .db    0   ; $b0
    .db    0   ; $b1
    .db    0   ; $b2
    .db    0   ; $b3
    .db    0   ; $b4
    .db    0   ; $b5
    .db    0   ; $b6
    .db    0   ; $b7
    .db    0   ; $b8
    .db    0   ; $b9
    .db    0   ; $ba
    .db    0   ; $bb
    .db    0   ; $bc
    .db    0   ; $bd
    .db    0   ; $be
    .db    0   ; $bf
    .db    0   ; $c0
    .db    0   ; $c1
    .db    0   ; $c2
    .db    0   ; $c3
    .db    0   ; $c4
    .db    0   ; $c5
    .db    0   ; $c6
    .db    0   ; $c7
    .db    0   ; $c8
    .db    0   ; $c9
    .db    0   ; $ca
    .db    0   ; $cb
    .db    0   ; $cc
    .db    0   ; $cd
    .db    0   ; $ce
    .db    0   ; $cf
    .db    0   ; $d0
    .db    0   ; $d1
    .db    0   ; $d2
    .db    0   ; $d3
    .db    0   ; $d4
    .db    0   ; $d5
    .db    0   ; $d6
    .db    0   ; $d7
    .db    0   ; $d8
    .db    0   ; $d9
    .db    0   ; $da
    .db    0   ; $db
    .db    0   ; $dc
    .db    0   ; $dd
    .db    0   ; $de
    .db    0   ; $df
    .db    0   ; $e0
    .db    0   ; $e1
    .db    0   ; $e2
    .db    0   ; $e3
    .db    0   ; $e4
    .db    0   ; $e5
    .db    0   ; $e6
    .db    0   ; $e7
    .db    0   ; $e8
    .db    0   ; $e9
    .db    0   ; $ea
    .db    0   ; $eb
    .db    0   ; $ec
    .db    0   ; $ed
    .db    0   ; $ee
    .db    0   ; $ef
    .db    0   ; $f0
    .db    0   ; $f1
    .db    0   ; $f2
    .db    0   ; $f3
    .db    0   ; $f4
    .db    0   ; $f5
    .db    0   ; $f6
    .db    0   ; $f7
    .db    0   ; $f8
    .db    0   ; $f9
    .db    0   ; $fa
    .db    0   ; $fb
    .db    0   ; $fc
    .db    0   ; $fd
    .db    0   ; $fe
    .db    0   ; $ff

ps2kbd_shifted_ascii:
    .db    0   ; $00
    .db    0   ; $01
    .db    0   ; $02
    .db    0   ; $03
    .db    $41 ; $04 KEY_A
    .db    $42 ; $05 KEY_B
    .db    $43 ; $06 KEY_C
    .db    $44 ; $07 KEY_D
    .db    $45 ; $08 KEY_E
    .db    $46 ; $09 KEY_F
    .db    $47 ; $0a KEY_G
    .db    $48 ; $0b KEY_H
    .db    $49 ; $0c KEY_I
    .db    $4a ; $0d KEY_J
    .db    $4b ; $0e KEY_K
    .db    $4c ; $0f KEY_L
    .db    $4d ; $10 KEY_M
    .db    $4e ; $11 KEY_N
    .db    $4f ; $12 KEY_O
    .db    $50 ; $13 KEY_P
    .db    $51 ; $14 KEY_Q
    .db    $52 ; $15 KEY_R
    .db    $53 ; $16 KEY_S
    .db    $54 ; $17 KEY_T
    .db    $55 ; $18 KEY_U
    .db    $56 ; $19 KEY_V
    .db    $57 ; $1a KEY_W
    .db    $58 ; $1b KEY_X
    .db    $59 ; $1c KEY_Y
    .db    $5a ; $1d KEY_Z
    .db    $21 ; $1e KEY_1
    .db    $40 ; $1f KEY_2
    .db    $23 ; $20 KEY_3
    .db    $24 ; $21 KEY_4
    .db    $25 ; $22 KEY_5
    .db    $5e ; $23 KEY_6
    .db    $26 ; $24 KEY_7
    .db    $2a ; $25 KEY_8
    .db    $28 ; $26 KEY_9
    .db    $29 ; $27 KEY_0
    .db    0   ; $28
    .db    0   ; $29
    .db    0   ; $2a
    .db    0   ; $2b
    .db    0   ; $2c
    .db    $5f ; $2d KEY_MINUS
    .db    $2b ; $2e KEY_EQUAL
    .db    $7b ; $2f KEY_LEFTBRACE
    .db    $7d ; $30 KEY_RIGHTBRACE
    .db    $7c ; $31 KEY_BACKSLASH
    .db    0   ; $32
    .db    $3a ; $33 KEY_SEMICOLON
    .db    $22 ; $34 KEY_APOSTROPHE
    .db    $7e ; $35 KEY_GRAVE
    .db    $3c ; $36 KEY_COMMA
    .db    $3e ; $37 KEY_DOT
    .db    $3f ; $38 KEY_SLASH
    .db    0   ; $39
    .db    0   ; $3a
    .db    0   ; $3b
    .db    0   ; $3c
    .db    0   ; $3d
    .db    0   ; $3e
    .db    0   ; $3f
    .db    0   ; $40
    .db    0   ; $41
    .db    0   ; $42
    .db    0   ; $43
    .db    0   ; $44
    .db    0   ; $45
    .db    0   ; $46
    .db    0   ; $47
    .db    0   ; $48
    .db    0   ; $49
    .db    0   ; $4a
    .db    0   ; $4b
    .db    0   ; $4c
    .db    0   ; $4d
    .db    0   ; $4e
    .db    0   ; $4f
    .db    0   ; $50
    .db    0   ; $51
    .db    0   ; $52
    .db    0   ; $53
    .db    0   ; $54
    .db    0   ; $55
    .db    0   ; $56
    .db    0   ; $57
    .db    0   ; $58
    .db    0   ; $59
    .db    0   ; $5a
    .db    0   ; $5b
    .db    0   ; $5c
    .db    0   ; $5d
    .db    0   ; $5e
    .db    0   ; $5f
    .db    0   ; $60
    .db    0   ; $61
    .db    0   ; $62
    .db    0   ; $63
    .db    0   ; $64
    .db    0   ; $65
    .db    0   ; $66
    .db    0   ; $67
    .db    0   ; $68
    .db    0   ; $69
    .db    0   ; $6a
    .db    0   ; $6b
    .db    0   ; $6c
    .db    0   ; $6d
    .db    0   ; $6e
    .db    0   ; $6f
    .db    0   ; $70
    .db    0   ; $71
    .db    0   ; $72
    .db    0   ; $73
    .db    0   ; $74
    .db    0   ; $75
    .db    0   ; $76
    .db    0   ; $77
    .db    0   ; $78
    .db    0   ; $79
    .db    0   ; $7a
    .db    0   ; $7b
    .db    0   ; $7c
    .db    0   ; $7d
    .db    0   ; $7e
    .db    0   ; $7f
    .db    0   ; $80
    .db    0   ; $81
    .db    0   ; $82
    .db    0   ; $83
    .db    0   ; $84
    .db    0   ; $85
    .db    0   ; $86
    .db    0   ; $87
    .db    0   ; $88
    .db    0   ; $89
    .db    0   ; $8a
    .db    0   ; $8b
    .db    0   ; $8c
    .db    0   ; $8d
    .db    0   ; $8e
    .db    0   ; $8f
    .db    0   ; $90
    .db    0   ; $91
    .db    0   ; $92
    .db    0   ; $93
    .db    0   ; $94
    .db    0   ; $95
    .db    0   ; $96
    .db    0   ; $97
    .db    0   ; $98
    .db    0   ; $99
    .db    0   ; $9a
    .db    0   ; $9b
    .db    0   ; $9c
    .db    0   ; $9d
    .db    0   ; $9e
    .db    0   ; $9f
    .db    0   ; $a0
    .db    0   ; $a1
    .db    0   ; $a2
    .db    0   ; $a3
    .db    0   ; $a4
    .db    0   ; $a5
    .db    0   ; $a6
    .db    0   ; $a7
    .db    0   ; $a8
    .db    0   ; $a9
    .db    0   ; $aa
    .db    0   ; $ab
    .db    0   ; $ac
    .db    0   ; $ad
    .db    0   ; $ae
    .db    0   ; $af
    .db    0   ; $b0
    .db    0   ; $b1
    .db    0   ; $b2
    .db    0   ; $b3
    .db    0   ; $b4
    .db    0   ; $b5
    .db    0   ; $b6
    .db    0   ; $b7
    .db    0   ; $b8
    .db    0   ; $b9
    .db    0   ; $ba
    .db    0   ; $bb
    .db    0   ; $bc
    .db    0   ; $bd
    .db    0   ; $be
    .db    0   ; $bf
    .db    0   ; $c0
    .db    0   ; $c1
    .db    0   ; $c2
    .db    0   ; $c3
    .db    0   ; $c4
    .db    0   ; $c5
    .db    0   ; $c6
    .db    0   ; $c7
    .db    0   ; $c8
    .db    0   ; $c9
    .db    0   ; $ca
    .db    0   ; $cb
    .db    0   ; $cc
    .db    0   ; $cd
    .db    0   ; $ce
    .db    0   ; $cf
    .db    0   ; $d0
    .db    0   ; $d1
    .db    0   ; $d2
    .db    0   ; $d3
    .db    0   ; $d4
    .db    0   ; $d5
    .db    0   ; $d6
    .db    0   ; $d7
    .db    0   ; $d8
    .db    0   ; $d9
    .db    0   ; $da
    .db    0   ; $db
    .db    0   ; $dc
    .db    0   ; $dd
    .db    0   ; $de
    .db    0   ; $df
    .db    0   ; $e0
    .db    0   ; $e1
    .db    0   ; $e2
    .db    0   ; $e3
    .db    0   ; $e4
    .db    0   ; $e5
    .db    0   ; $e6
    .db    0   ; $e7
    .db    0   ; $e8
    .db    0   ; $e9
    .db    0   ; $ea
    .db    0   ; $eb
    .db    0   ; $ec
    .db    0   ; $ed
    .db    0   ; $ee
    .db    0   ; $ef
    .db    0   ; $f0
    .db    0   ; $f1
    .db    0   ; $f2
    .db    0   ; $f3
    .db    0   ; $f4
    .db    0   ; $f5
    .db    0   ; $f6
    .db    0   ; $f7
    .db    0   ; $f8
    .db    0   ; $f9
    .db    0   ; $fa
    .db    0   ; $fb
    .db    0   ; $fc
    .db    0   ; $fd
    .db    0   ; $fe
    .db    0   ; $ff

    .endif ; seen_keycodes
