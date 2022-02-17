/* bb 2022 */

                                /* dot (.) represents directive */
.thumb                          /* assembler will interpret instructions as T32 instructions, using the UAL syntax. */
.cpu cortex-m3

.thumb_func
.global _start                  /* modify the ELF binding of a symbol, sets the binding of _start to STB_GLOBAL */
_start:
stacktop: .word 0x20001000      
.word reset                     /* allocates 4B of memory in the current section, and define the initial contents of that memory. */

.thumb_func
reset:
    bl main
    b .

.thumb_func
.globl PUT32
PUT32:
    str r1,[r0]
    bx lr

.thumb_func
.globl GET32
GET32:
    ldr r0,[r0]
    bx lr

.thumb_func
.globl DUMMY
DUMMY:
    bx lr
