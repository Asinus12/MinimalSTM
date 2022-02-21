#!/usr/bin/sh

xxd_cmd="xxd build/main.bin"
nm_cmd="arm-none-eabi-nm build/main.elf"
dumpt_cmd="arm-none-eabi-objdump -t build/main.elf"
dumph_cmd="arm-none-eabi-objdump -h build/main.elf"


echo "Running: $xxd_cmd"
eval "$xxd_cmd"

echo "Running: $nm_cmd"
eval "$nm_cmd"

echo "Running: $dumpt_cmd"
eval "$dumpt_cmd"

$echo "Running: $dumph_cmd"
eval "$dumph_cmd"