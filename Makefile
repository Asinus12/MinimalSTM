# Name of the binaries.
PROJ_NAME=minimalSTM

# base folder
BASE_DIR=.
BUILD_DIR =./build

######################################################################
#                         SETUP SOURCES                              #
######################################################################
SRCS   = main.c
SRCS += altmain.c
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
SIZE 	= $(TOOLS_DIR)/arm-none-eabi-size
XXD		= $(TOOLS_DIR)/xxd


## Assembler options
AFLAGS = --warn --fatal-warnings -mcpu=cortex-m3

## Compiler options 
#CFLAGS  = -ggdb # WILL NOT FLASH PROPERLY 
CFLAGS = -Wall -O0 
CFLAGS = -Wextra -Warray-bounds -Wextra -Warray-bounds
CFLAGS += -ffreestanding # does not include libc
CFLAGS += -mcpu=cortex-m3 -mthumb
CFLAGS += -mlittle-endian -mthumb-interwork
#CFLAGS += -mfloat-abi=hard -mfpu=fpv4-sp-d16


## Linker options
LFLAGS = -Tflash.ld 
#LFLAGS += --specs=nano.specs
#LFLAGS  = -nostdlib -nostartfiles 





######################################################################
#                         SETUP TARGETS                              #
######################################################################

.PHONY: $(PROJ_NAME)
$(PROJ_NAME): $(PROJ_NAME).elf

$(PROJ_NAME).elf: $(SRCS)
	$(CC) $(CFLAGS) $(LFLAGS) $^ -o $@ 
	$(OBJCOPY) -O ihex $ $(PROJ_NAME).elf $(PROJ_NAME).hex
	$(OBJCOPY) -O binary $(PROJ_NAME).elf $(PROJ_NAME).bin

#	$(CC) $(CFLAGS) -s startup.s -c $(BUILD_DIR)/startup.o 
# 	$(AS) $(AFLAGS) startup.s -o $(BUILD_DIR)/startup.o
# 	$(CC) $(CFLAGS) -c main.c -o $(BUILD_DIR)/main.o
# 	$(CC) $(CFLAGS) -c altmain.c -o $(BUILD_DIR)/altmain.o
# 	$(LD) $(LFLAGS) $(BUILD_DIR)/*.o -o $(BUILD_DIR)/$(PROJ_NAME).elf
# 	$(OBJDUMP) -D $(BUILD_DIR)/$(PROJ_NAME).elf > $(BUILD_DIR)/$(PROJ_NAME).list
# 	$(OBJCOPY) -O binary $(BUILD_DIR)/$(PROJ_NAME).elf $(BUILD_DIR)/$(PROJ_NAME).bin
# 	$(SIZE) $(BUILD_DIR)/$(PROJ_NAME).elf



# Clean the project 
clean:
#	rm -rf  $(BUILD_DIR)/*
	rm -f *.o $(PROJ_NAME).elf $(PROJ_NAME).hex $(PROJ_NAME).bin

# Flash the STM32
flash: 
	st-flash write $(PROJ_NAME).bin 0x8000000

# Show sections, sizes, VMAs and LMAs 
dumpmain:
	$(XXD) $(BUILD_DIR)/$(PROJ_NAME).elf
	$(OBJDUMP) -h $(BUILD_DIR)/$(PROJ_NAME).elf
	$(SIZE) $(BUILD_DIR)/$(PROJ_NAME).elf
