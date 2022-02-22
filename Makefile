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
OBJDUMP = $(TOOLS_DIR)/arm-non-eabi-objdump
OBJCOPY = $(TOOLS_DIR)/arm-none-eabi-objcopy


## Assembler options
AFLAGS = --warn --fatal-warnings -mcpu=cortex-m3

## Compiler options 
CFLAGS = -Wall -O2 -ffreestanding
CFLAGS += -mcpu=cortex-m3 -mthumb

## Linker options
LFLAGS  = -nostdlib -nostartfiles -T




######################################################################
#                         SETUP TARGETS                              #
######################################################################

.PHONY: $(PROJ_NAME)

$(PROJ_NAME): $(PROJ_NAME).elf

$(PROJ_NAME).elf: $(SRCS)
	$(AS) $(AFLAGS) startup.s -o $(BUILD_DIR)/startup.o
	$(CC) $(CFLAGS) -c main.c -o $(BUILD_DIR)/main.o
	$(LD) $(LFLAGS) flash.ld $(BUILD_DIR)/*.o -o $(BUILD_DIR)/$(PROJ_NAME).elf
	$(OBJCOPY) -O binary $(BUILD_DIR)/$(PROJ_NAME).elf $(BUILD_DIR)/$(PROJ_NAME).bin



clean:
	rm -f *.o $(PROJ_NAME).elf $(PROJ_NAME).hex $(PROJ_NAME).bin

# Flash the STM32
flash: 
	st-flash write $(BUILD_DIR)/$(PROJ_NAME).bin 0x8000000