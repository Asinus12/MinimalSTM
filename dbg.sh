#!/usr/bin/sh

xxd_cmd="xxd build/notmain.bin"
nm_cmd="arm-none-eabi-nm build/notmain.elf"
dump_cmd="arm-none-eabi-objdump -h build/notmain.elf"

echo "Running: $xxd_cmd"
eval "$xxd_cmd"

echo "Running: $nm_cmd"
eval "$nm_cmd"

echo "Running: $dump_cmd"
eval "$dump_cmd"