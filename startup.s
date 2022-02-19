/* both .global and .globl are accepted for compatibility */

.thumb
.thumb_func

.global _start
_start:

stacktop: .word 0x20001000

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
