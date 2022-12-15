CC := arm-none-eabi-gcc
INC = -Iinclude

CFLAGS = -O2  \
	 -mcpu=cortex-m4 -mthumb \
	 -g -nostartfiles -ffunction-sections  -Wall -Wextra -static -nostdlib  \

SRC:=$(wildcard ./src/**/*.c) $(wildcard ./src/*.c)
OBJ:=$(wildcard ./build/*.o)

TARGET = devices
all: $(TARGET)

$(TARGET): clean compile_c move_objects move_objects_to_main

move_objects_to_main:
	cp ./build/*.o ../../build

move_objects:
	mv ./*.o ./build

compile_c: $(SRC)
	$(CC) $(CFLAGS) $(INC) $^ -c 

clean:
	rm -f ./build/*
