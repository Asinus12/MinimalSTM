/* bb 2022 */

/* dot (.) represents directive */
/* .global modifies the ELF binding of a symbol to STB_GLOBAL */
/* .thumb makes assembler interpret instructions as T32, using the UAL syntax. */
/* .word allocates 4 Bytes, .byte 1Byte, .hword 2Bytes, .quad 8Bytes, .octa 8Bytes */
/*  str r3, [r1] ... stores the value found in r3 to location of r1 */
.thumb                          
.cpu cortex-m3



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

/*
.global LOOP
LOOP:
   ldr r7,=0x200000
label:
   sub r7,#1
   bne label
   bx lr      
*/


.global LEDON
LEDON:
    ldr r1, =0x40011010 
    ldr r3, =0x2000
    str r3, [r1]
    bx lr

.global LEDOFF
LEDOFF: 
    ldr r1, =0x40011010 
    ldr r3, =0x20000000
    str r3, [r1]
    bx lr


