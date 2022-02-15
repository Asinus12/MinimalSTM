
# MinimalSTM #  


## Compilation ## 

From root directory run: 

```
$ rm -rf build
$ mkdir build
$ arm-none-eabi-as --warn --fatal-warnings -mcpu=cortex-m3 flash.s -o ./build/flash.o
$ arm-none-eabi-gcc -Wall -O2 -ffreestanding -mcpu=cortex-m3 -mthumb -c notmain.c -o ./build/notmain.o
$ arm-none-eabi-ld -nostdlib -nostartfiles -T flash.ld ./build/flash.o ./build/notmain.o -o ./build/notmain.elf
$ arm-none-eabi-objdump -D ./build/notmain.elf > ./build/notmain.list
$ arm-none-eabi-objcopy -O binary ./build/notmain.elf ./build/notmain.bin
```
From build directory run: 

```
$ st-flash write notmain.bin 0x08000000
```



## Testing ## 
- Tested on  STM32F103C8T6 (Bluepill) medium density device with 20KiB SRAM, 64KiB Flash (1Kib pages)

## Bugs and notes ## 



## Other ##