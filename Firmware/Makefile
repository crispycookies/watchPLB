###############################################################################
#
#  	    File        : Makefile
#
#       Abstract    : Example Makefile for a C Project
#
#       Environment : Atollic TrueSTUDIO(R)
#
###############################################################################

SHELL=cmd

# System configuration
CC = arm-atollic-eabi-gcc
RM=rm -rf

# Assembler, Compiler and Linker flags and linker script settings
LINKER_FLAGS=-lm -mthumb -mcpu=cortex-m0plus  -Wl,--gc-sections -T$(LINK_SCRIPT) -static  -Wl,--start-group -lc -lm -Wl,--end-group -specs=nano.specs -specs=nosys.specs  -Wl,-cref "-Wl,-Map=$(BIN_DIR)/WatchPLB.map" -Wl,--defsym=malloc_getpagesize_P=0x1000
LINK_SCRIPT="stm32_flash.ld"
ASSEMBLER_FLAGS=-c -g -O0 -mcpu=cortex-m0plus  -mthumb -specs=nano.specs -D"STM32L073xx"  -x assembler-with-cpp  -Isrc -IDrivers/CMSIS/Include -IDrivers/CMSIS/Device/ST/STM32L0xx/Include -IDrivers/STM32L0xx_HAL_Driver/Inc
COMPILER_FLAGS=-c -g -mcpu=cortex-m0plus  -O0 -Wall -ffunction-sections -fdata-sections -mthumb -specs=nano.specs -D"STM32L073xx"   -Isrc -IDrivers/CMSIS/Include -IDrivers/CMSIS/Device/ST/STM32L0xx/Include -IDrivers/STM32L0xx_HAL_Driver/Inc

# Define output directory
OBJECT_DIR = Debug
BIN_DIR = $(OBJECT_DIR)

# Define sources and objects
SRC := $(wildcard src/*.c) \
	$(wildcard src/*/*.c) \
	Drivers/STM32L0xx_HAL_Driver/Src/stm32l0xx_hal_gpio.c
SRCSASM := 	$(wildcard */*/*/*/*/*/*/*/*.s) \
	$(wildcard */*/*/*/*/*/*/*.s) \
	$(wildcard */*/*/*/*/*/*.s) \
	$(wildcard */*/*/*/*/*.s) \
	$(wildcard */*/*/*/*.s) \
	$(wildcard */*/*/*.s) \
	$(wildcard */*/*.s) \
	$(wildcard */*.s)
OBJS := $(SRC:%.c=$(OBJECT_DIR)/%.o) $(SRCSASM:%.s=$(OBJECT_DIR)/%.o)
OBJS := $(OBJS:%.S=$(OBJECT_DIR)/%.o)  

###############
# Build project
# Major targets
###############
all: buildelf

buildelf: $(OBJS) 
	$(CC) -o "$(BIN_DIR)/WatchPLB.elf" $(OBJS) $(LINKER_FLAGS)

clean:
	$(RM) $(OBJS) "$(BIN_DIR)/WatchPLB.elf" "$(BIN_DIR)/WatchPLB.map"


##################
# Specific targets
##################
$(OBJECT_DIR)/src/main.o: src/main.c
	@mkdir $(subst /,\,$(dir $@)) 2> NUL || echo off
	$(CC) $(COMPILER_FLAGS) src/main.c -o $(OBJECT_DIR)/src/main.o 


##################
# Implicit targets
##################
$(OBJECT_DIR)/%.o: %.c
	@mkdir $(subst /,\,$(dir $@)) 2> NUL || echo off
	$(CC) $(COMPILER_FLAGS) $< -o $@

$(OBJECT_DIR)/%.o: %.s
	@mkdir $(subst /,\,$(dir $@)) 2> NUL || echo off
	$(CC) $(ASSEMBLER_FLAGS) $< -o $@
	
$(OBJECT_DIR)/%.o: %.S
	@mkdir $(subst /,\,$(dir $@)) 2> NUL || echo off
	$(CC) $(ASSEMBLER_FLAGS) $< -o $@