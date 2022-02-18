/* bb 2022 */

/* dot (.) represents directive */
/* .global modifies the ELF binding of a symbol to STB_GLOBAL */
/* .thumb makes assembler interpret instructions as T32, using the UAL syntax. */
/* .word allocates 4 Bytes, .byte 1Byte, .hword 2Bytes, .quad 8Bytes, .octa 8Bytes */

.thumb                          
.cpu cortex-m3



.section .text.vector, "ax", %progbits
vector:
    Loop:
    b Loop;
.size vector, .-vector

 stacktop: .word 0x20001000  

.word reset                     
reset:
    bl main
    b .

.global PUT32
PUT32:
    str r1,[r0]
    bx lr

.global GET32
GET32:
    ldr r0,[r0]
    bx lr

.global DUMMY
DUMMY:
    bx lr
