PROGRAM := m3-boot
BUILD_PATH := build

SOURCES := main.c \
           mmu.c \
           uart.c

CFLAGS   := -O0 -mcpu=cortex-a9 -march=armv7-a \
            -nostdlib -fno-builtin -nostartfiles -nodefaultlibs -Wall \
            -Werror -I../common
ASFLAGS  := -mcpu=cortex-a9 -march=armv7-a
LDFLAGS  := -Tlink.ld -nostdlib -fno-builtin -nostartfiles -nodefaultlibs \
            -mcpu=cortex-a9 -march=armv7-a -Wall

CC       := arm-none-eabi-gcc
AS       := arm-none-eabi-gcc
OBJCOPY  := arm-none-eabi-objcopy

# Rules

OBJECTS := $(SOURCES:%.c=$(BUILD_PATH)/%.o)
DEPS := $(OBJECTS:%.o=%.d)


.PHONY: all install clean CREATE_BUILD_PATH

all: build

build: CREATE_BUILD_PATH $(BUILD_PATH)/$(PROGRAM).img

clean:
	-rm -rf $(BUILD_PATH)

CREATE_BUILD_PATH:
	mkdir -p $(BUILD_PATH)

$(BUILD_PATH)/%.o: %.c
	$(CC) $(CFLAGS) -MMD -MP -MF $(@:%.o=%.d) -MT $@ -o $@ -c $<

$(BUILD_PATH)/%.o: %.S
	$(AS) $(ASFLAGS) -MMD -MP -MF $(@:%.o=%.d) -MT $@ -o $@ -c $<

$(BUILD_PATH)/$(PROGRAM).img: $(BUILD_PATH)/$(PROGRAM).bin
	mkimage -A arm -C none -O linux -T kernel -a 0x9E000000 -e 0x9E000000 -d $< $@

$(BUILD_PATH)/$(PROGRAM).bin: $(BUILD_PATH)/$(PROGRAM).elf
	$(OBJCOPY) -O binary $< $@

$(BUILD_PATH)/$(PROGRAM).elf: $(OBJECTS)
	$(CC) $(LDFLAGS) -o $@ $(OBJECTS) -Wl,-Map,$(BUILD_PATH)/$(PROGRAM).map

-include $(DEPS)

