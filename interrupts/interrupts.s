    .include "prelude.inc"

start:
    ; Lets see what's in the registers at startup
    pha
    pla

    phx
    plx

    phy
    ply

    php
    plp

end:
    bra end

    .include "vectors.inc"
