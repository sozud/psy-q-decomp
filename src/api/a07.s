    SECTION .rdata
    SECTION .text

    xdef	DeliverEvent

DeliverEvent
    addiu       t2, zero, $B0
    jr          t2
    addiu       t1, zero, $7
    nop

    SECTION .data
    SECTION .sdata
    SECTION .bss
    SECTION .sbss
