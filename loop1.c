#include "tinyasm.c"

    START("loop1.csv");

declare(loop_top);
    equ(LOOPS,10);
declare(loop_done);
declare(outer);
label(outer);
    addi(r1,r0,LOOPS);
    nand(r2,r0,r0);
label(loop_top);
    add(r1,r1,r2);
    beq(r1,r0,loop_done);
    beq(r0,r0,loop_top);
label(loop_done);
//    beq(r1,r1,outer);
    halt();
    END

