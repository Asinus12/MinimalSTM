echo "building ..."

rm -rf build
mkdir build

arm-none-eabi-as --warn --fatal-warnings -mcpu=cortex-m3 flash.s -o ./build/flash.o
arm-none-eabi-gcc -Wall -O2 -ffreestanding -mcpu=cortex-m3 -mthumb -c notmain.c -o ./build/notmain.o
arm-none-eabi-ld -nostdlib -nostartfiles -T flash.ld ./build/flash.o ./build/notmain.o -o ./build/notmain.elf
arm-none-eabi-objdump -D ./build/notmain.elf > ./build/notmain.list
arm-none-eabi-objcopy -O binary ./build/notmain.elf ./build/notmain.bin

echo "Finished! Check build directory" 
