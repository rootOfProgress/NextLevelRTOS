CC := arm-none-eabi-gcc
AS := arm-none-eabi-as
INC = -Iinclude
LINK = link.ld

#-mapcs-frame -funwind-tables 
CFLAGS = -O2  \
	 -mcpu=cortex-m4 -mthumb \
	 -g -nostartfiles -ffunction-sections  -Wall -Wextra -static -nostdlib  \

# SRC=$(shell find $('./src') -name '*.c')
# OBJ:=$(shell find $('./build/') -name '*.o')
SRC:=$(wildcard ./src/**/*.c) $(wildcard ./src/*.c)
OBJ:=$(wildcard ./build/*.o)

SRC_AS=$(shell find $('./src') -name '*.s')
TARGET = firmware
all: $(TARGET)

$(TARGET): compile_c move_objects compile_as link

link1: link
# $(TARGET): compile_as

move_objects:
	mv *.o ./build

compile_c: $(SRC)
	$(CC) $(CFLAGS) $(INC) $^ -c 

compile_as: $(SRC_AS)
	$(AS) $^ -o ./build/asm.o

link: $(wildcard ./build/*.o) $(wildcard ./legacy/*.o)
	echo $^
	$(CC) -T $(LINK) -L ./libfoo.a -ffunction-sections $^ -o ./build/$(TARGET) -nostdlib -nostartfiles

clean:
	rm -f ./build/*
