echo "building ..."

rm -rf build
mkdir build

arm-none-eabi-as --warn --fatal-warnings -mcpu=cortex-m3 startup.s -o ./build/startup.o
arm-none-eabi-gcc -Wall -O2 -ffreestanding -mcpu=cortex-m3 -mthumb -c main.c -o ./build/main.o
arm-none-eabi-ld -nostdlib -nostartfiles -T flash.ld ./build/startup.o ./build/main.o -o ./build/main.elf
arm-none-eabi-objdump -D ./build/main.elf > ./build/main.list
arm-none-eabi-objcopy -O binary ./build/main.elf ./build/notmain.bin

echo "Finished! Check build directory" 
