#include "tinyasm.c"

    START("loop2.csv");

declare(loop_top);
    equ(LOOPS,10);
declare(loop_done);

    addi(r1,r0,LOOPS);
    nand(r2,r0,r0);
    lea(r3,loop_top);
label(loop_top);
    add(r1,r1,r2);
    beq(r1,r0,loop_done);
    jalr(r7,r3);
label(loop_done);
    halt();
    END

