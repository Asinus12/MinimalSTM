
# MinimalSTM #    
**Minimal bare-metal project for exploring Cortex-MR architecture**  
Project is built on  STM32F103C8T6 (RM0041) medium density device with 20KiB SRAM, 64KiB Flash.

**References**
- Linker script reference: https://sourceware.org/binutils/docs/ld/index.html
- Assembly directives (armclang): https://developer.arm.com/documentation/100067/0612/armclang-Integrated-Assembler/Macro-directives 
- Tool Interface Standard (TIS) Executable and Linking Format (ELF) Specification Version 1.2
- ARM Procedure Call Standard 
- https://interrupt.memfault.com/tag/zero-to-main/
- Memory protection unit: https://interrupt.memfault.com/blog/fix-bugs-and-secure-firmware-with-the-mpu  
- GNU assembler directives : https://docs.huihoo.com/redhat/rhel-4-docs/rhel-as-en-4/index.html

**Building:**
```
sudo ./run.sh
```
**Uploading**
```
$ st-flash write ./build/main.bin 0x08000000
```

**Dumping object files (.o, .elf)**  
```
$ xxd build/main.elf
```
```
$ arm-none-eabi-nm build/main.elf 
```
```
$ arm-none-eabi-objdump -h build/main.elf 
```
```
$ arm-none-eabi-objdump -t build/main.elf | sort 
```

**Debugging with gdb-multiarch and st-util**

Set up a gdb server (default port 4242):  
```
  $ st-util  
```

Run gdb-multiarch and connect to server:  
```
  $ gdb-multiarch main.elf
  $ (gdb) target remote localhost:4242
```
GDB commands:
```
  $ layout next ... changes perspective 
  $ b main (b 73) ... sets breakpoint at main (line 73)
  $ continue, next(dont dive), step(dive)
  $ finish ... continue until current fucntion returns 

  $ info line * 0x08000000 ... disassembly of memory location
  $ info mem 
  $ info threads
  $ display, undisplay <num>
  $ i r ... shows general purpose registers with CSPR 
  $ backtrace full
  $ kill run
```    







## CORTEX M3 BOOT ## 

- CortexM3 can only boot from 0x0 from reset! However vector table can be relocated during program execution by writing to VTOR (0xE000ED08). Note that vector table entries for Cortex-M3 are address values and not branch instructions like traditional ARM cores. (ARMDeveloper)  
- Boot-up of C-M3 is different to traditional ARM cores, in that the first fetch from address 0x0 is the initial value of the SP, the second value is reset handler, i.e. the starting address of program code.  
 

Boot modes depend on voltages on pin BOOT1 and BOOT0: 
```    
BOOT1     BOOT0         Boot mode:  
  x         0             main flash  
  0         1             system memory (bootloader)  
  1         1             sram  
```    

CPU:  
1) Reads BOOT0 and BOOT1 to determine boot mode 
2) Fetches MSP address from address 0x00000000 (_estack)
3) Fetches PC from address 0x00000004, PC always holds the address of the next instruction to be exectued! (Reset_handler). Note that LSB of Reset_handler address indicates thumb mode and is always 1. 
4) Reset_Handler initializes data and bss segment, initializes some hardware and calls the maing function. 





## LINKER SCRIPT (flash.ld) ## 

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




## ASSEMBLY STARTUP FILE (startup.s) ##
- Written in UAL-ARM assembly  

Example ADD(register): developer.arm.com  
```
ADD{S}<c><q>  {<Rd>,} <Rn>, <Rm> {,<shift>}

15  14  13  12  11  10  09  08  07  06  05  04  03  02  01
 0   0   0   1   1   0   0  Rm          Rn          Rd
```

- ```{ } ```... optional fields
- ``` |``` ... alternative character string, LDM|STM is either LDM or STM  
- ```*``` ... when used in a commbination like ```<immed_8> * 4```, the value must be a multiple of 4 in the range 4*0=0 to 4*255=1020  
- ```<c>```... condition under which the instruction is executed, if ommited defaults to AL
- ```<q>```... qualifier can be either .N (narrow), or .W (wide)
- ```Rd``` ... destination register, if ommited is same as Rn
- ```Rn``` ... first operand 
- ```Rm``` ... second operand also optionally shifted register 
- ```<shift>``` ... shift to apply to the value read from ```Rm```



**Instructions** 
```
ldr r0, =_sdata ... loads r0 with symbol 
adds r1, r1, #4 .. loads r1 with value from address [r1+4] (relative addressing)
bcc CopyDataInit ... Branch if Carry Clear
```

## BOOTLOADER ##

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