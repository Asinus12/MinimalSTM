/* location counter (.) only appears inside sections, allways tracks VMA */



STACK_SIZE = 0x80; /* 128   */
PC13_BSRR = 0x40011010;
_endstack = 0x20005000;

_Min_Heap_Size = 300;      /* required amount of heap  */
_Min_Stack_Size = 0; /* required amount of stack */

MEMORY
{
    rom : ORIGIN = 0x08000000, LENGTH = 64K
    ram : ORIGIN = 0x20000000, LENGTH = 20K 
}

SECTIONS
{   
    

    .isr_vector :          /* vectors go at start of flash */
    {
        _svector = . ;
        . = ALIGN(4);
         KEEP(*(.isr_vector)) 
        . = ALIGN(4);
        _evector = . ;
    } >rom


    .text :                 /* code, data */
    { 
        *(.text*)           /* wildcard * means we want all .text section in all files */
    } > rom


   
    .bss (NOLOAD) :         /* uninitialized data */   
    {
        _sbss = . ;  
        __bss_start__ = _sbss;       
        *(.bss*)
        *(COMMON)           /* int foo() yes, static int foo() no*/
        _ebss = . ;   
        __bss_end__ = _ebss;      
    } > ram


    
    .data :                 /* initializeed static varibles*/            
    {
        _sdata = . ;
        *(.data*);
        _edata = . ;
    } > ram AT >rom         /* "VMA AT LMA */


    .stack (NOLOAD) :
    {
        . = ALIGN(4);
        . = . + STACK_SIZE;
        . = ALIGN(4);
    } > ram

    ._user_heap_stack :
    {
        . = ALIGN(4);
        PROVIDE ( end = . );
        PROVIDE ( _end = . );
        . = . + _Min_Heap_Size;
        . = . + _Min_Stack_Size;
        . = ALIGN(4);
    } >ram


}
