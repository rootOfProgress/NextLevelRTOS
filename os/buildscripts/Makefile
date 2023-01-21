CC := arm-none-eabi-gcc
LINK = ../arch/$(ARCH)/link.ld

# CFLAGS = -O2  \
#          -mcpu=cortex-m4 -mthumb \
#          -g -nostartfiles -ffunction-sections  -Wall -Wextra -static -nostdlib  \

LFLAGS = -nostdlib -nostartfiles
OBJ:=$(wildcard ../build/*.o)

ifeq (0,$(HWFP))
LFLAGS += -lgcc
endif

TARGET = $(ARCH)
all: link

link: $(OBJ)
	$(CC) -T $(LINK) -ffunction-sections $^  -o ../build/firmware $(LFLAGS)
#// $((if (0,$(HWFP)), "-lgcc"))