# Configuration

PROGRAM := quadcopter
BUILD_PATH := build

SOURCES := main.c \
           sensors.c \
           debug.c \
           quadcopter.c \
           angle.c \
           vector.c \
           pwm.c \
           command.c

CFLAGS   := -Os -g3 -gdwarf-2  -mcpu=cortex-m3 -mthumb -march=armv7-m \
            -nostdlib -ffunction-sections -fdata-sections \
            -Wl,--gc-sections -Wall -Werror -I$(MAPLE_ROOT)/libmaple \
            -DBOARD_maple_native -DMCU_STM32F103ZE -DERROR_LED_PORT=GPIOC \
            -DERROR_LED_PIN=15 -DSTM32_HIGH_DENSITY -DSERIAL_DEBUG
ASFLAGS  := -mcpu=cortex-m3 -march=armv7-m -mthumb \
            -x assembler-with-cpp
LDFLAGS  := -Llink -mcpu=cortex-m3 -mthumb -Xlinker \
            --gc-sections -march=armv7-m -Wall \
            -L$(MAPLE_ROOT)/build

CC       := arm-none-eabi-gcc
AS       := arm-none-eabi-gcc
OBJCOPY  := arm-none-eabi-objcopy
DFU      := dfu-util

# Rules

OBJECTS := $(SOURCES:%.c=$(BUILD_PATH)/%.o)
DEPS := $(OBJECTS:%.o=%.d)

MAPLE_OBJS := $(shell find $(MAPLE_ROOT)/build/libmaple -iname "*.o")
MAPLE_OBJS_FULL := $(MAPLE_OBJS:%=$(MAPLE_ROOT)/build/%)

.PHONY: all install install_flash install_ram build_flash build_ram clean CREATE_BUILD_PATH

all: build_ram build_flash

install: install_ram

build_flash: CREATE_BUILD_PATH $(BUILD_PATH)/$(PROGRAM)_flash

build_ram: CREATE_BUILD_PATH $(BUILD_PATH)/$(PROGRAM)_ram

install_flash:
	./reset.py && \
	sleep 1 && \
	$(DFU) -a1 -d 1EAF:0003 -D $(BUILD_PATH)/$(PROGRAM)_flash -R

install_ram:
	./reset.py && \
	sleep 1 && \
	$(DFU) -a0 -d 1EAF:0003 -D $(BUILD_PATH)/$(PROGRAM)_ram -R

clean:
	-rm -Rf $(BUILD_PATH)

CREATE_BUILD_PATH:
	mkdir -p $(BUILD_PATH)

$(BUILD_PATH)/%.o: %.c
	$(CC) $(CFLAGS) -MMD -MP -MF $(@:%.o=%.d) -MT $@ -o $@ -c $<

$(BUILD_PATH)/%.o: %.S
	$(AS) $(ASFLAGS) -MMD -MP -MF $(@:%.o=%.d) -MT $@ -o $@ -c $<

$(BUILD_PATH)/$(PROGRAM)_flash.elf: $(OBJECTS)
	$(CC) $(LDFLAGS) -Tmaple_native_flash.ld -o $@ $(OBJECTS) $(MAPLE_OBJS) -Wl,-Map,$(BUILD_PATH)/$(PROGRAM).map

$(BUILD_PATH)/$(PROGRAM)_ram.elf: $(OBJECTS)
	$(CC) $(LDFLAGS) -Tmaple_native_ram.ld -o $@ $(OBJECTS) $(MAPLE_OBJS) -Wl,-Map,$(BUILD_PATH)/$(PROGRAM).map

$(BUILD_PATH)/$(PROGRAM)_flash: $(BUILD_PATH)/$(PROGRAM)_flash.elf
	$(OBJCOPY) -v -Obinary $(BUILD_PATH)/$(PROGRAM)_flash.elf $@ 1>/dev/null

$(BUILD_PATH)/$(PROGRAM)_ram: $(BUILD_PATH)/$(PROGRAM)_ram.elf
	$(OBJCOPY) -v -Obinary $(BUILD_PATH)/$(PROGRAM)_ram.elf $@ 1>/dev/null

-include $(DEPS)


