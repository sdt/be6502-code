    .include "prelude.inc"

start:
    .blk 32752, $ea

    jmp start

    .include "vectors.inc"
