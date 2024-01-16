#
#  Illusense Inc.
#      Eduardo Rhod 06/01/2021
#

TARGET = $(notdir $(CURDIR))
#
# library
#
LIB_ROOT = ../STM32CubeH7/Drivers
BSP = $(LIB_ROOT)/BSP/STM32H7xx_Nucleo
CMSIS = $(LIB_ROOT)/CMSIS/Device/ST/STM32H7xx/Include
CMSIS_INCL = $(LIB_ROOT)/Include
HAL_DRV = $(LIB_ROOT)/STM32H7xx_HAL_Driver/Src
HAL_DRV_INC = $(LIB_ROOT)/STM32H7xx_HAL_Driver/Inc
STARTUP_FILE = $(CURDIR)/src/Startup
CORE_INC = $(LIB_ROOT)/CMSIS/Include
MCU_SPEC  = cortex-m7

BUILDDIR = build
SOURCES := $(shell find -name '*.c')
HEADER_FILES := $(shell find -name '*.h')
# LIBSOURCES += $(shell find $(HAL_DRV) -name '*.c')
# LIBSOURCES += $(shell find $(CMSIS) -name '*.c')
SOURCES += $(shell find $(HAL_DRV) -name '*.c')
SOURCES += $(shell find $(CMSIS) -name '*.c')
HEADERS := $(sort $(dir $(shell find -name '*.h')))
OBJS := $(subst ./,./build/,$(SOURCES:%.c=%.o))
LIBOBJS := $(subst ./,./build/,$(LIBSOURCES:%.c=%.o))

ELF = $(TARGET).elf
HEX = $(TARGET).hex
BIN = $(TARGET).bin
HAL_LIB = libstm32h7drv.a

# search path
vpath %.c $(BSP)
vpath %.c $(CMSIS)
vpath %.h $(CMSIS)
vpath %.h $(CMSIS_INCL)
vpath %.h $(HAL_DRV_INC)
vpath %.c $(HAL_DRV)
vpath %.s $(STARTUP_FILE)

# Toolchain definitions (ARM bare metal defaults)
GCC_ARM_DIR = /home/eduardo/embedded/gcc-arm-none-eabi-10-2020-q4-major
CC = $(GCC_ARM_DIR)/bin/arm-none-eabi-gcc
AS = $(GCC_ARM_DIR)/bin/arm-none-eabi-as
LD = $(GCC_ARM_DIR)/bin/arm-none-eabi-ld
OC = $(GCC_ARM_DIR)/bin/arm-none-eabi-objcopy
OD = $(GCC_ARM_DIR)/bin/arm-none-eabi-objdump
OS = $(GCC_ARM_DIR)/bin/arm-none-eabi-size
DB = $(GCC_ARM_DIR)/bin/arm-none-eabi-gdb
# Stlink programmer
STLINK_ROOT = /home/eduardo/embedded/stlink-1.7.0

# Assembly directives.
ASFLAGS += -c
ASFLAGS += -O3
ASFLAGS += -mcpu=$(MCU_SPEC)
ASFLAGS += -mthumb
ASFLAGS += -Wall
# (Set error messages to appear on a single line.)
ASFLAGS += -fmessage-length=0
FWSHA = "1234abc"
# C compilation directives
CFLAGS += -mcpu=$(MCU_SPEC)
CFLAGS += -mthumb
CFLAGS += -Wall
# (Set error messages to appear on a single line.)
CFLAGS += -fmessage-length=0
CFLAGS += -D ILLU_BOARD_ID=$(BOARD_ID)

# Included to support including different calibration files according to the board ID
#includes
BOARD_FILES_DIR = $(CURDIR)/src/Inc/board$(BOARD_ID)
BOARD_FILES := $(shell find $(BOARD_FILES_DIR) -maxdepth 1 -name '*.h')
CFLAGS += -I$(BSP) -I$(CMSIS) -I$(CMSIS_INCL) \
-I$(HAL_DRV_INC) -I$(CURDIR)/Inc -I$(CORE_INC) \
-I$(CURDIR)/src/Inc -DSTM32H735xx



# Included after bin size error while programming
CFLAGS += -ffunction-sections -fdata-sections -fstack-usage --specs=nano.specs -mfpu=fpv5-d16 -mfloat-abi=hard

# Linker directives.
LSCRIPT = $(shell find -name '*FLASH.ld')
LFLAGS += -mcpu=$(MCU_SPEC)
LFLAGS += -mthumb
LFLAGS += -Wall
# LFLAGS += -specs=nosys.specs
# LFLAGS += -nostdlib
LFLAGS += -lgcc
LFLAGS += -T$(LSCRIPT)
LFLAGS += -L$(BSP) -L$(CMSIS) -L$(CMSIS_INCL) \
-L$(HAL_DRV_INC) -L$(CURDIR)/Inc -L$(CORE_INC)

# Included after bin size error  while programming
LFLAGS += --specs=nosys.specs -Wl,-Map=$(TARGET).map -Wl,--gc-sections -static --specs=nano.specs -mfpu=fpv5-d16 -mfloat-abi=hard -mthumb -Wl,--start-group -lc -lm -Wl,--end-group

# Startup asm
AS_SRC   =  $(shell find -name '*.s')
STARTUP_O = $(AS_SRC:.s=.o)
OBJS += $(STARTUP_O)

#update the define FW_SHA value
ifndef BOARD_ID
$(error BOARD_ID is not set. Please pass the parameter 'BOARD_ID=N' when runing make where N is equal to the board number)
endif

.PHONY: all
all: $(BIN) $(HEX)
	$(info )
	$(info ******* Making for board id = $(BOARD_ID) ********)

# @echo "***** Making for board Id="$(BOARD_ID) "*****"
.PHONY: update_board_files
update_board_files: $(BOARD_FILES)
	cp -f $(BOARD_FILES) $(CURDIR)/src/Inc

$(STARTUP_O): $(AS_SRC)
	$(CC) $(ASFLAGS) $< -o $@

# .o .c target
$(BUILDDIR)/%.o: %.c $(HEADER_FILES)
	@mkdir -p $(@D)
	$(CC) -c $(CFLAGS) $< -o $@
	# $(CC) -MM $(CFLAGS) $< > $*.d

# .o .s target
$(BUILDDIR)/%.o: %.s
	@mkdir -p $(@D)
	$(CC) -c $(CFLAGS) $< -o $@

$(ELF): $(OBJS)
	$(CC) $^ $(LFLAGS) -o $@

$(BIN): $(ELF)
	$(OC) -O binary $< $@
	$(OS) $<

%.hex: %.elf
	$(OC) -O ihex $< $@

.PHONY: flash
flash: update_sha update_board_files $(BIN)
	$(STLINK_ROOT)/st-flash write $(BIN) 0x8000000

.PHONY: flash_hex
flash_hex: $(HEX)
	$(STLINK_ROOT)/st-flash --format ihex write $(HEX)

.PHONY: debug
debug: update_board_files update_cflags clean $(ELF)
	$(DB) $(ELF) --tui

.PHONY: run_debug
run_debug:
	$(DB) $(ELF) --tui

.PHONY: run_debug_auto
run_debug_auto:
	$(DB) --batch --command=gdb.cmd $(ELF) > debug.txt

.PHONY: gdb_server
gdb_server:
	$(STLINK_ROOT)/st-util &

.PHONY: update_cflags
update_cflags:
	$(eval CFLAGS += -g)

.PHONY: update_sha
update_sha:
	$(info "Updating SHA")
	./fw_sha_update.sh

.PHONY: clean
clean:
	rm -f $(OBJS)
	rm -f $(ELF)
	rm -f $(BIN)
	rm -f $(HEX)


