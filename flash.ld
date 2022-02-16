

STACK_SIZE = 0x200; /* 512 B */

MEMORY
{
    rom : ORIGIN = 0x08000000, LENGTH = 64K
    ram : ORIGIN = 0x20000000, LENGTH = 20K 
}

SECTIONS
{
    .text :                 /* code, data */
    { 
        KEEP(*(.vector*))   /* .vector section contains function we want to keep */
                            /* at the very start of .text section so reset handler can find them */
        *(.text*)           /* wildcard * means we want all .text section in all files */
    } > rom


    .bss (NOLOAD) :         /* uninitialized data, ram is not loaded with anything! */ 
    {
        _sbss = . ;         /* symbol = location counter, makes section address avaliable from code */
        *(.bss*)
        *(COMMON)           /* int foo goes here but static int foo does not */
        _ebss = . ;         /* access _ebss symbol from code ... uint8_t *data_byte = &_sdata; */
    } > ram


    .data :                 /* initializeed static varibles copied from rom before main is called */
    {
        _sdata = . ;
        *(.data*);
        _edata = . ;
    } > ram AT >rom         /* "> ram" is the VMA, "> rom" is the LMA */


    .stack (NOLOAD) :
    {
        . = ALIGN(8);
        . = . + STACK_SIZE;
        . = ALIGN(8);
    } > ram




}
