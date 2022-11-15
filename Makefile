CC := arm-none-eabi-gcc
CFLAGS = -fno-common -O2 \
	 -mcpu=cortex-m4 -mthumb \
	 -T link1.ld -g -nostartfiles -Wall -Wextra -I./include \

SRC=$(shell find $('./src') -name '*.c')
TARGET = firmware
all: $(TARGET)
#$(SRCS) $(SRCS1) 
$(TARGET): $(SRC) | build
	$(CC) $(CFLAGS) $^ -o build/firmware.elf

build:
	mkdir -p $@

clean:
	rm -f *.o *.bin *.elf
