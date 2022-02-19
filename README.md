
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
$ arm-none-eabi-as --warn --fatal-warnings -mcpu=cortex-m3 startup.s -o ./build/startup.o
$ arm-none-eabi-gcc -Wall -O2 -ffreestanding -mcpu=cortex-m3 -mthumb -c main.c -o ./build/main.o
$ arm-none-eabi-ld -nostdlib -nostartfiles -T flash.ld ./build/flash.o ./build/main.o -o ./build/main.elf
$ arm-none-eabi-objdump -D ./build/main.elf > ./build/main.list
$ arm-none-eabi-objcopy -O binary ./build/main.elf ./build/main.bin
```

**Flashing** 
```
$ st-flash write ./build/main.bin 0x08000000
```

**Debuging**  
```
$ xxd build/main.bin  | head ... shows addresses starting at 0x00000000 and their values 
$ arm-none-eabi-nm build/main.elf ... shows assigned address of implemented function in object files
$ arm-none-eabi-objdump -h build/main.elf  ... show sections, sizes, VMA/LMA addresses and allignment 
$ arm-none-eabi-objdump -t build/main.elf | sort ... dumps symbols, addresses, and sections 
```

**Debugging with gdb-multiarch and st-util**
```
  $ st-util  ... sets up a server gdb server on port (4242)
  $ gdb-multiarch main.elf
  $ target remote localhost:4242 ... in gdb
  $ b main ... sets breakpoint at main
  $ b 73 ... sets breakpoint at line 73
  $ continue
  $ next .. dont dive 
  $ step .. dive into 
  $ finish ... continue until current fucntion returns 
  $ info line * 0x08000000 ... disassembly of memory location
  $ info mem 
  $ info threads
  $ display, undisplay <num>
  $ i r ... shows general purpose registers with CSPR 
  $ backtrace full
  $ kill run
```    



**References**
- Linker script reference: https://sourceware.org/binutils/docs/ld/index.html
- Assembly directives (armclang): https://developer.arm.com/documentation/100067/0612/armclang-Integrated-Assembler/Macro-directives 
- Tool Interface Standard (TIS) Executable and Linking Format (ELF) Specification Version 1.2
- ARM Procedure Call Standard 
- https://interrupt.memfault.com/tag/zero-to-main/
- Memory protection unit: https://interrupt.memfault.com/blog/fix-bugs-and-secure-firmware-with-the-mpu


## Linker script ## 

Linker script is responsible for:  
    - **Memory layout**: what memory is available where  
    - **Section definitions**: what part of a program should go where  
    - **Options**: commands to specify architecture, entry point, …etc. if needed  
    - **Symbols**: variables to inject into the program at link time  

**Explanation of used keywords**
- PROVIDE(symbol = expression) ... provide keyword may be used to define a symbol, such as      'etext', only if its referenced but not defined  
- HIDDEN(symbol = expression) ... for ELF targeted ports, symbol will be hidden and not exported
- ENTRY(symbol) ... first instruction that executes in a program is called a entry point, argument is a symbol name.
- LONG(addr) ... stores four byte value of symbol addr, location counter is increased, inside the section! (BYTE(1) SHORT(2) LONG(4) QUAD(8))
- NOLOAD ... section is loaded with noload property (such as ram)
- ALIGN sets a special variable "location counter".  
- (LMA) load memory address, (VMA) virtual memory address.  
  } > ram AT > rom  // "> ram" is the VMA, "> rom" is the LMA 
  In a firmware context, the LMA is where your JTAG loader needs to place the section and the VMA
  is where the section is found during execution.
- (.) represents the value of the location counter 
- Do not use \DISCARD\ for any sections, its a reserved keyword!



**linker script section anatomy**
```
section [address] [(type)] :
  [AT(lma)]
  [ALIGN(section_align) | ALIGN_WITH_INPUT]
  [SUBALIGN(subsection_align)]
  [constraint]
  {
    output-section-command
    output-section-command
    …
  } [>region] [AT>lma_region] [:phdr :phdr …] [=fillexp] [,]
```




## Assembly startup file ##

**Instructions** 
```
ldr r0, =_sdata ... loads r0 with symbol 
adds r1, r1, #4 .. loads r1 with value from address [r1+4] (relative addressing)
bcc CopyDataInit ... Branch if Carry Clear
```

## Bootloader ##

  Bootloader's job is to copy code from non-executable storage (SPI flash, flash) to executable storage (RAM),  
  it can contain firmware update logic so device can recover from a bug and also offers bigger security  (cryptographic signature)

**The goal of our bootloader is:**
- Execute on MCU boot  
- Jump to applicaton code   




Make sure that a CPU is in privileged mode  

16kB reserved for bootloader ( it must end on a flash boundary region)  
```
        0x0 +---------------------+
            |                     |
            |     Bootloader      |
            |                     |
     0x4000 +---------------------+
            |                     |
            |                     |
            |     Application     |
            |                     |
            |                     |
    0x30000 +---------------------+

```

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