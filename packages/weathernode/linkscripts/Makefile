CC := arm-none-eabi-gcc
LINK = ../link_external.ld

CFLAGS = -O0  \
         -mcpu=cortex-m4 -mthumb \
         -g -nostartfiles -ffunction-sections  -Wall -Wextra -static -nostdlib  \

OBJ:=$(wildcard ../build/*.o)

all: link

link: $(OBJ)
	$(CC) \
	-L/usr/share/arm-gnu-toolchain-12.2.mpacbti-bet1-x86_64-arm-none-eabi/arm-none-eabi/lib/thumb/v7e-m/nofp \
	-L/usr/share/arm-gnu-toolchain-12.2.mpacbti-bet1-x86_64-arm-none-eabi/lib/gcc/arm-none-eabi/12.2.0/thumb/v7e-m/nofp/ \
	-L/home/benni/development/NextLevelRTOS/packages/lib/archives \
	-T $(LINK) \
    $^ -o ../build/$(TARGET) -nostdlib -nostartfiles -lm -lgcc ../../lib/archives/*.a
