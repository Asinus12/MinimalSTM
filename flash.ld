/* location counter (.) only appears inside sections, allways tracks VMA */

STACK_SIZE = 0x80; /* 128   */

MEMORY
{
    rom : ORIGIN = 0x08000000, LENGTH = 64K
    ram : ORIGIN = 0x20000000, LENGTH = 20K 
}

SECTIONS
{   
    /* code, data */
    .text :                 
    { 
        *(.text*)           /* wildcard * means we want all .text section in all files */
    } > rom

    /* uninitialized data */ 
    .bss (NOLOAD) :         
    {
        _sbss = . ;         
        *(.bss*)
        *(COMMON)           /* int foo goes here but static int foo does not */
        _ebss = . ;         /* access _ebss symbol from code ... uint8_t *data_byte = &_sdata; */
    } > ram

    /* initializeed static varibles*/
    .data :                 
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
    



}
