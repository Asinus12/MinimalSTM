
# MinimalSTM #    
Minimal bare-metal project for exploring and microbenchmarking STM's microcontrollers. 


## Building and dependancies  ## 
- Project is built on  STM32F103C8T6 (Bluepill) medium density device with 20KiB SRAM, 64KiB Flash (1Kib pages)
- STM reference manual RM0041 STM32F100xx
- For building executables and binaries you need to have ***arm-none-eabi*** installed  
- For uploading to MCU you need ***st-link*** or other flashig tool installed (STM flash utility etc)   


**Building**
```
$ rm -rf build
$ mkdir build
$ arm-none-eabi-as --warn --fatal-warnings -mcpu=cortex-m3 flash.s -o ./build/flash.o
$ arm-none-eabi-gcc -Wall -O2 -ffreestanding -mcpu=cortex-m3 -mthumb -c notmain.c -o ./build/notmain.o
$ arm-none-eabi-ld -nostdlib -nostartfiles -T flash.ld ./build/flash.o ./build/notmain.o -o ./build/notmain.elf
$ arm-none-eabi-objdump -D ./build/notmain.elf > ./build/notmain.list
$ arm-none-eabi-objcopy -O binary ./build/notmain.elf ./build/notmain.bin
```

**Flashing** 
```
$ st-flash write ./build/notmain.bin 0x08000000
```

**Debuging**  
- arm-none-eabi-nm notmain.elf ... shows assigned address of implemented function in object files
- arm-none-eabi-objdump -t build/notmain.elf ... 
- arm-none-eabi-objdump -h build/notmain.o  ... show what sections are in our object file 
- arm-none-eabi-objdump -t build/notmain.elf | sort ... dump symbols

**References**
- Tool Interface Standard (TIS) Executable and Linking Format (ELF) Specification Version 1.2
- ARM Procedure Call Standard 
- https://interrupt.memfault.com/blog/zero-to-main-1
- Binutilis reference: https://sourceware.org/binutils/docs/ld/Source-Code-Reference.html
- Memory protection unit: https://interrupt.memfault.com/blog/fix-bugs-and-secure-firmware-with-the-mpu



## Linker script ## 

Linker script is responsible for:
    - Memory layout: what memory is available where
    - Section definitions: what part of a program should go where
    - Options: commands to specify architecture, entry point, …etc. if needed
    - Symbols: variables to inject into the program at link time


- Do not use \DISCARD\ for any sections, its a reserved keyword! 
- .vector sections contains functions we want to KEEP at the start of the .text sections
  so that Reset_Hanlder is where the MCU expects it to be 
- NOLOAD ... section is loaded with noload property (only property in modern linker scripts)
- ALIGN sets a special variable "location counter"
- STACK has also noload property (beacaue its in ram), has no sysmbols so we have to define size and
  align on 8-byte boundary (ARM procedure call standard) 
- Every section in our linker script has two addresses, its load address (LMA) and its virtual address (VMA). 
  In a firmware context, the LMA is where your JTAG loader needs to place the section and the VMA
  is where the section is found during execution.
- (.) represents the value of the location counter 

  ...} > ram AT > rom  // "> ram" is the VMA, "> rom" is the LMA 

**linker script template**
```
MEMORY
{
  rom      (rx)  : ORIGIN = 0x00000000, LENGTH = 0x00040000
  ram      (rwx) : ORIGIN = 0x20000000, LENGTH = 0x00008000
}

STACK_SIZE = 0x2000;

/* Section Definitions */
SECTIONS
{
    /* data, code */
    .text :
    {
        KEEP(*(.vectors .vectors.*))
        *(.text*)
        *(.rodata*)
    } > rom

    /* uninitialized data */
    .bss (NOLOAD) :
    {
        *(.bss*)
        *(COMMON)
    } > ram

    /* initialized data */
    .data :
    {
        *(.data*);
    } > ram AT >rom

    /* stack section */
    .stack (NOLOAD):
    {
        . = ALIGN(8);
        . = . + STACK_SIZE;
        . = ALIGN(8);
    } > ram

    _end = . ;
}
```




## Assembly startup file ##

todo

## Bootloader ##

  Bootloader's job is to copy code from non-executable storage (SPI flash, flash) to executable storage (RAM),  
  it can contain firmware update logic so device can recover from a bug and also offers bigger security  (cryptographic signature)

**The goal of our bootloader is:**
- Execute on MCU boot  
- Jump to applicaton code   
- It must end on a flash sector boundary 

```
/* memory_map.ld */
MEMORY
{
  bootrom  (rx)  : ORIGIN = 0x00000000, LENGTH = 0x00004000
  approm   (rx)  : ORIGIN = 0x00004000, LENGTH = 0x0003C000
  ram      (rwx) : ORIGIN = 0x20000000, LENGTH = 0x00008000
}

__bootrom_start__ = ORIGIN(bootrom);
__bootrom_size__ = LENGTH(bootrom);
__approm_start__ = ORIGIN(approm);
__approm_size__ = LENGTH(approm);
```


To make them accesible in C, we declare them in a header file 
```
* memory_map.h */
#pragma once

extern int __bootrom_start__;
extern int __bootrom_size__;
extern int __approm_start__;
extern int __approm_size__;
```