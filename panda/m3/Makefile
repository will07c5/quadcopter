PROGRAM := m3-image
BUILD_PATH := build

SOURCES := main.c \
           intr.c \
           handlers.c \
           uart.c \
           debug.c \
           i2c.c \
           sensors.c \
           vector.c \
           pwm.c \
           command.c \
           angle.c \
           pid.c \
           control.c

COMMON_SRCS := ringbuf.c

CFLAGS   := -O0 -mcpu=cortex-m3 -mthumb -march=armv7-m \
            -nostdlib -Wall -Wextra -Werror -I../common 
ASFLAGS  := -mcpu=cortex-m3 -march=armv7-m -mthumb
LDFLAGS  := -Tlink.ld -mcpu=cortex-m3 -mthumb -march=armv7-m -Wall \
            -nostdlib
CC       := arm-none-eabi-gcc
AS       := arm-none-eabi-gcc
OBJCOPY  := arm-none-eabi-objcopy

# Rules

OBJECTS := $(SOURCES:%.c=$(BUILD_PATH)/%.o)
COMMON_OBJECTS := $(COMMON_SRCS:%.c=$(BUILD_PATH)/common/%.o)
DEPS := $(OBJECTS:%.o=%.d) $(COMMON_OBJECTS:%.o=%.d)

.PHONY: all install clean CREATE_BUILD_PATH

all: build

build: CREATE_BUILD_PATH $(BUILD_PATH)/$(PROGRAM).bin

clean:
	-rm -Rf $(BUILD_PATH)

CREATE_BUILD_PATH:
	mkdir -p $(BUILD_PATH); mkdir -p $(BUILD_PATH)/common

$(BUILD_PATH)/%.o: %.c
	$(CC) $(CFLAGS) -MMD -MP -MF $(@:%.o=%.d) -MT $@ -o $@ -c $<

$(BUILD_PATH)/common/%.o: ../common/%.c
	$(CC) $(CFLAGS) -MMD -MP -MF $(@:%.o=%.d) -MT $@ -o $@ -c $<

$(BUILD_PATH)/%.o: %.S
	$(AS) $(ASFLAGS) -MMD -MP -MF $(@:%.o=%.d) -MT $@ -o $@ -c $<

$(BUILD_PATH)/$(PROGRAM).bin: $(BUILD_PATH)/$(PROGRAM).elf
	$(OBJCOPY) -O binary $< $@

$(BUILD_PATH)/$(PROGRAM).elf: $(OBJECTS) $(COMMON_OBJECTS)
	$(CC) $(LDFLAGS) -o $@ $(OBJECTS) $(COMMON_OBJECTS) -Wl,-Map,$(BUILD_PATH)/$(PROGRAM).map

-include $(DEPS)

