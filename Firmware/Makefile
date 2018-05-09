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

HAL_DIRECTORY=Drivers/STM32L0xx_HAL_Driver
HAL_PREFIX=stm32l0xx_hal_

# Assembler, Compiler and Linker flags and linker script settings
LINKER_FLAGS=-lm -mthumb -mcpu=cortex-m0plus  -Wl,--gc-sections -T$(LINK_SCRIPT) -static  -Wl,--start-group -lc -lm -Wl,--end-group -specs=nano.specs -specs=nosys.specs  -Wl,-cref "-Wl,-Map=$(BIN_DIR)/WatchPLB.map" -Wl,--defsym=malloc_getpagesize_P=0x1000
LINK_SCRIPT="stm32_flash.ld"
ASSEMBLER_FLAGS=-c -g -O0 -mcpu=cortex-m0plus  -mthumb -specs=nano.specs -D"STM32L073xx"  -x assembler-with-cpp
COMPILER_FLAGS=-c -g -mcpu=cortex-m0plus  -O0 -Wall -ffunction-sections -fdata-sections -mthumb -specs=nano.specs -D"STM32L073xx" -include stm32l0xx_hal_conf.h
INCLUDES=-IDrivers/CMSIS/Include \
	-IDrivers/CMSIS/Device/ST/STM32L0xx/Include \
	-I$(HAL_DIRECTORY)/Inc \
	-IDrivers/User/adc \
	-IDrivers/User/key \
	-IDrivers/User/led \
	-IDrivers/User/rfTx \
	-IDrivers/User/uart \
	-IDrivers/User/watchdog \
	-IDrivers/Interfaces/ble \
	-IDrivers/Interfaces/gps \
	-IDrivers/Interfaces/usb \
	-IApp/communication \
	-IApp/emergencyCall \
	-IApp/location \
	-IApp/system \
	-IApp/userInterface
	
HAL_MODULES=gpio adc dma tim usart

# Define output directory
OBJECT_DIR = Debug
BIN_DIR = $(OBJECT_DIR)

# Define sources and objects
SRC := $(wildcard App/*/*.c) \
	$(wildcard Drivers/User/*/*.c) \
	$(wildcard Drivers/Interfaces/*/*.c)
SRCSASM := App/main/startup_stm32l073xx.s
OBJS := $(SRC:%.c=$(OBJECT_DIR)/%.o) $(HAL_MODULES:%=$(OBJECT_DIR)/$(HAL_DIRECTORY)/Src/$(HAL_PREFIX)%.o) $(SRCSASM:%.s=$(OBJECT_DIR)/%.o)
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
# Implicit targets
##################
$(OBJECT_DIR)/%.o: %.c
	@mkdir $(subst /,\,$(dir $@)) 2> NUL || echo off
	$(CC) $(COMPILER_FLAGS) $(INCLUDES) $< -o $@

$(OBJECT_DIR)/%.o: %.s
	@mkdir $(subst /,\,$(dir $@)) 2> NUL || echo off
	$(CC) $(ASSEMBLER_FLAGS) $(INCLUDES) $< -o $@
	
$(OBJECT_DIR)/%.o: %.S
	@mkdir $(subst /,\,$(dir $@)) 2> NUL || echo off
	$(CC) $(ASSEMBLER_FLAGS) $(INCLUDES) $< -o $@
	