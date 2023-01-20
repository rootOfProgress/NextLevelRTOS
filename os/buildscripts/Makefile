CC := arm-none-eabi-gcc
LINK = ../arch/$(ARCH)/link.ld

# CFLAGS = -O2  \
#          -mcpu=cortex-m4 -mthumb \
#          -g -nostartfiles -ffunction-sections  -Wall -Wextra -static -nostdlib  \

OBJ:=$(wildcard ../build/*.o)

TARGET = $(ARCH)
all: link

link: $(OBJ)
	$(CC) -T $(LINK) -ffunction-sections $^  -o ../build/firmware -nostdlib -nostartfiles