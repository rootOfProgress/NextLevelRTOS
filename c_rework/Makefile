CC := arm-none-eabi-gcc
AS := arm-none-eabi-as
INC = -Iinclude
LINK = link.ld

CFLAGS = -fno-common -O0 \
	 -mcpu=cortex-m4 -mthumb \
	 -g -nostartfiles -Wall -Wextra \

SRC=$(shell find $('./src') -name '*.c')
OBJ=$(shell find $('./build') -name '*.o')
SRC_AS=$(shell find $('./src') -name '*.s')
TARGET = firmware
all: $(TARGET)

$(TARGET): clean compile_c move_objects compile_as link
# $(TARGET): compile_as

move_objects:
	mv *.o ./build

compile_c: $(SRC)
	$(CC) $(CFLAGS) $(INC) $^ -c 

compile_as: $(SRC_AS)
	$(AS) $^ -o ./build/asm.o

link: $(OBJ)
	$(CC) -T $(LINK) $^ -o ./build/$(TARGET) -nostdlib -nostartfiles
#//-o ./build/
# build:
# mkdir -p $@

clean:
	rm -f ./build/*
