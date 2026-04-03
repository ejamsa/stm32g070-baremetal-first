CC = arm-none-eabi-gcc
CFLAGS  ?=  -W -Wall -Wextra -Werror -Wundef -Wshadow -Wdouble-promotion \
            -Wformat-truncation -fno-common -Wconversion \
            -g3 -O0 -ffunction-sections -fdata-sections -I. \
            -mcpu=cortex-m0plus -mthumb -mfloat-abi=soft $(EXTRA_CFLAGS)
LDFLAGS ?= -Tlinker_script.ld -mcpu=cortex-m0plus -nostartfiles -nostdlib --specs nano.specs -lc -lgcc -Wl,--gc-sections -Wl,-Map=$@.map
OBJS = main.o 
OUTPUT_DIR = out
OUTPUT_FILENAME = firmware
ELFFILE = $(OUTPUT_FILENAME).elf

RM = rm -f

vpath %.c src
vpath %.h src

.PHONY: build oflash debug clean

build: $(OUTPUT_FILENAME).bin

main.c: hal.h

firmware.elf: $(OBJS)
	arm-none-eabi-gcc $^ $(LDFLAGS) -o $@

firmware.bin: $(ELFFILE)
	arm-none-eabi-objcopy -O binary $< $@

oflash: $(ELFFILE)
	openocd -f interface/stlink.cfg -f target/stm32g0x.cfg -c "program $< verify reset exit"

debug: $(ELFFILE)
	openocd -f interface/stlink.cfg -f target/stm32g0x.cfg & arm-none-eabi-gdb $< 

clean:
	$(RM) $(OUTPUT_FILENAME).* *.o

