CC := arm-none-eabi-gcc
AS := arm-none-eabi-as
INC = -Iinclude
LINK = link.ld

CFLAGS = -O2  \
	 -mcpu=cortex-m4 -mthumb \
	 -g -nostartfiles -ffunction-sections -Wall -Wextra -static -nostdlib  \
	 -Wno-implicit-fallthrough

SRC:=$(wildcard ./src/**/*.c) $(wildcard ./src/*.c)
OBJ:=$(wildcard ./build/*.o)

SRC_AS=$(wildcard ./src/**/*.s) $(wildcard ./src/*.s)
TARGET = firmware
all: $(TARGET)

$(TARGET): clean compile_c $(ARCH) link

$(ARCH):
	make -C arch/$(ARCH)

move_objects:
	mv *.o ./build

compile_c: $(SRC)
	$(CC) $(CFLAGS) $(INC) -c $^ 
	mv *.o ./build

compile_as: $(SRC_AS)
	$(AS) $^ -o ./build/asm.o

link: 
	make -C ./build ARCH=$(ARCH)

clean:
	rm -f ./build/*.o $(TARGET)
