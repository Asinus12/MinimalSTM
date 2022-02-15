.thumb

.thumb_func
.global _start
_start:
stacktop: .word 0x20001000
.word reset

.thumb_func
reset:
    bl notmain
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
