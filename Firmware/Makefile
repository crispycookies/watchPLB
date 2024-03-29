###############################################################################
#
#  	    File        : Makefile
#
#       Abstract    : Example Makefile for a C Project
#
#       Environment : Atollic TrueSTUDIO(R)
#
###############################################################################

ifeq ($(OS),Windows_NT)
	SHELL=cmd
	MKDIR=@mkdir $(subst /,\,$(dir $@)) 2> NUL || echo off
else
	SHELL=/bin/sh
	MKDIR=@mkdir -p $(dir $@)
endif


# System configuration
CC = arm-atollic-eabi-gcc
RM=rm -rf

HAL_DIRECTORY=Drivers/STM32L0xx_HAL_Driver
HAL_PREFIX=stm32l0xx_hal_

# Assembler, Compiler and Linker flags and linker script settings
LINKER_FLAGS=-lm -mthumb -mcpu=cortex-m0plus  -Wl,--gc-sections -T$(LINK_SCRIPT) -static  -Wl,--start-group -lc -lm -Wl,--end-group -specs=nosys.specs  -Wl,-cref "-Wl,-Map=$(BIN_DIR)/WatchPLB.map" -Wl,--defsym=malloc_getpagesize_P=0x1000
LINK_SCRIPT="stm32_flash.ld"
ASSEMBLER_FLAGS=-c -g -O0 -mcpu=cortex-m0plus  -mthumb -D"STM32L073xx"  -x assembler-with-cpp
COMPILER_FLAGS=-c -g -mcpu=cortex-m0plus  -O0 -Wall -ffunction-sections -fdata-sections -mthumb -D"STM32L073xx" -include stm32l0xx_hal_conf.h -include system_stm32l0xx.h -include stm32l0xx_hal.h -include logger.h
INCLUDES= \
	-ITools/BitArray \
	-ITools/Logger \
	-IDrivers/CMSIS/Include \
	-IDrivers/CMSIS/Device/ST/STM32L0xx/Include \
	-I$(HAL_DIRECTORY)/Inc \
	-IDrivers/User/adc \
	-IDrivers/User/key \
	-IDrivers/User/led \
	-IDrivers/User/radio \
	-IDrivers/User/uart \
	-IDrivers/User/sysclock \
	-IDrivers/User/dma \
	-IDrivers/User/forceFeedback \
	-IDrivers/User/spi \
	-IDrivers/User/usb \
	-IDrivers/User/watchdog \
	-IDrivers/User/sysclock \
	-IDrivers/Interfaces/battery \
	-IDrivers/Interfaces/ble/CRC \
	-IDrivers/Interfaces/ble \
	-IDrivers/Interfaces/nmea \
	-IDrivers/Interfaces/log \
	-IDrivers/Interfaces/ubx \
	-IDrivers/Interfaces/position \
	-IDrivers/Interfaces/plb \
	-IDrivers/Interfaces/battery \
	-IApp/communication \
	-IApp/emergencyCall \
	-IApp/location \
	-IApp/system \
	-IApp/userInterface
	
HAL_MODULES=gpio adc dma tim uart uart_ex rcc rcc_ex cortex flash pwr pcd pcd_ex spi iwdg wwdg

# Define output directory
OBJECT_DIR = Debug
BIN_DIR = $(OBJECT_DIR)

# Define sources and objects
SRC := $(wildcard App/*/*.c) \
	$(wildcard Drivers/User/*/*.c) \
	$(wildcard Drivers/Interfaces/*/*.c) \
	$(wildcard Tools/*/*.c) \
	$(HAL_DIRECTORY)/Src/stm32l0xx_hal.c

INC := $(wildcard *.h) \
	$(wildcard */*.h) \
	$(wildcard */*/*.h) \
	$(wildcard */*/*/*.h)
	
SRCSASM := App/main/startup_stm32l073xx.s
OBJS := $(SRC:%.c=$(OBJECT_DIR)/%.o) $(HAL_MODULES:%=$(OBJECT_DIR)/$(HAL_DIRECTORY)/Src/$(HAL_PREFIX)%.o) $(SRCSASM:%.s=$(OBJECT_DIR)/%.o)
OBJS := $(OBJS:%.S=$(OBJECT_DIR)/%.o) 

###############
# Build project
# Major targets
###############
all: buildelf

buildelf: $(OBJS) $(INC)
	$(CC) -o "$(BIN_DIR)/WatchPLB.elf" $(OBJS) $(LINKER_FLAGS)

clean:
	$(RM) $(OBJS) "$(BIN_DIR)/WatchPLB.elf" "$(BIN_DIR)/WatchPLB.map"
	
##################
# Implicit targets
##################
$(OBJECT_DIR)/%.o: %.c $(INC)
	$(MKDIR)
	$(CC) $(COMPILER_FLAGS) $(INCLUDES) $< -o $@

$(OBJECT_DIR)/%.o: %.s $(INC)
	$(MKDIR)
	$(CC) $(ASSEMBLER_FLAGS) $(INCLUDES) $< -o $@
	
$(OBJECT_DIR)/%.o: %.S $(INC)
	$(MKDIR)
	$(CC) $(ASSEMBLER_FLAGS) $(INCLUDES) $< -o $@