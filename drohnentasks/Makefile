CC := arm-none-eabi-gcc
INC = -Iinclude

CFLAGS = -O0  \
	 -mcpu=cortex-m4 -mthumb \
	 -g -nostartfiles -ffunction-sections  -Wall -Wextra -static -nostdlib -mfloat-abi=hard \

SRC:=$(wildcard ./src/**/*.c) $(wildcard ./src/*.c)
OBJ:=$(wildcard ./build/*.o)

TARGET = devices
all: $(TARGET)

$(TARGET): clean compile move

compile: $(SRC)
	$(CC) $(CFLAGS) $(INC) $^ -c 

move:
	mv *.o ../os/build

clean:
	rm -f ./build/*
