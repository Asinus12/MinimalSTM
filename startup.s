/* startup.s (as - GNU Assembler) https://docs.huihoo.com/redhat/rhel-4-docs/rhel-as-en-4/index.html */
/* @ ... can be used as a comment that extends to the end of the line */
/* ; ... can be used instead of a newline */
/* #, $ ... indicated immediate operand */
/* .thumb ... Performs the same action .code 16,  */
/* .thumb indicates T32 with UAL-ARM syntax */
/* .globl ... both .global and .globl are accepted for compatibility */
/* .thumb_func ... this directive specifies that the following symbol is the name of a Thumb encoded function */




.thumb                      
.thumb_func

.global _start
_start:

/* Highest address of the user mode stack */
/* _estack = 0x20005000;    /* end of RAM */
stacktop: .word 0x20005000



.word reset
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


.thumb_func
.globl LEDON
LEDON:
    ldr r1, =0x40011010 
    ldr r3, =0x2000
    str r3, [r1]
    bx lr


.thumb_func
.globl LEDOFF
LEDOFF: 
    ldr r1, =0x40011010 
    ldr r3, =0x20000000
    str r3, [r1]
    bx lr

.thumb_func
.globl LOOP
LOOP:
   ldr r7,=0x200000
label:
   sub r7,#1
   bne label
   bx lr      

