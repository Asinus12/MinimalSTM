# Name of the binaries.
PROJ_NAME=minimalSTM

# base folder
BASE_DIR=.
BUILD_DIR =./build

######################################################################
#                         SETUP SOURCES                              #
######################################################################

SRCS   = main.c
SRCS += startup.s

######################################################################
#                         SETUP TOOLS                                #
######################################################################

TOOLS_DIR = /usr/bin

AS		= $(TOOLS_DIR)/arm-none-eabi-as
CC      = $(TOOLS_DIR)/arm-none-eabi-gcc
LD		= $(TOOLS_DIR)/arm-none-eabi-ld
OBJDUMP = $(TOOLS_DIR)/arm-none-eabi-objdump
OBJCOPY = $(TOOLS_DIR)/arm-none-eabi-objcopy


## Assembler options
AFLAGS = --warn --fatal-warnings -mcpu=cortex-m3

## Compiler options 
CFLAGS = -Wall -O2 -ffreestanding
CFLAGS += -mcpu=cortex-m3 -mthumb

## Linker options
LFLAGS  = -nostdlib -nostartfiles 




######################################################################
#                         SETUP TARGETS                              #
######################################################################

.PHONY: $(PROJ_NAME)

	mkdir $(BUILD_DIR)
$(PROJ_NAME): $(PROJ_NAME).elf

$(PROJ_NAME).elf: $(SRCS)
	$(AS) $(AFLAGS) startup.s -o $(BUILD_DIR)/startup.o
	$(CC) $(CFLAGS) -c main.c -o $(BUILD_DIR)/main.o
	$(LD) $(LFLAGS) -T flash.ld $(BUILD_DIR)/startup.o $(BUILD_DIR)/main.o -o $(BUILD_DIR)/$(PROJ_NAME).elf
	$(OBJDUMP) -D $(BUILD_DIR)/$(PROJ_NAME).elf > $(BUILD_DIR)/$(PROJ_NAME).list
	$(OBJCOPY) -O binary $(BUILD_DIR)/$(PROJ_NAME).elf $(BUILD_DIR)/$(PROJ_NAME).bin



clean:
	rm -rf  $(BUILD_DIR)/*

# Flash the STM32
flash: 
	st-flash write $(BUILD_DIR)/$(PROJ_NAME).bin 0x8000000