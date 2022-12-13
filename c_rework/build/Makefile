CC := arm-none-eabi-gcc
LINK = ../arch/$(ARCH)/link.ld

CFLAGS = -O2  \
	 -mcpu=cortex-m4 -mthumb \
	 -g -nostartfiles -ffunction-sections  -Wall -Wextra -static -nostdlib  \

OBJ:=$(wildcard ./*.o)

TARGET = firmware
all: link

link: $(OBJ)
	$(CC) -T $(LINK) -ffunction-sections $^ -o ./$(TARGET) -nostdlib -nostartfiles
