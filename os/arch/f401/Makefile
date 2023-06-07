CC := arm-none-eabi-gcc
INC = -Iinclude

CFLAGS = -O3  \
	 -mcpu=cortex-m4 -mthumb \
	 -g -nostartfiles -ffunction-sections  -Wall -Wextra -static -nostdlib  \

SRC:=$(wildcard ./src/**/*.c) $(wildcard ./src/*.c)
OBJ:=$(wildcard ./build/*.o)

TARGET = devices
all: $(TARGET)

$(TARGET): clean compile_platform move_objects

ifeq (1,$(HWFP))
CFLAGS += -mfloat-abi=hard
else
CFLAGS += -mfloat-abi=soft
endif

move_objects:
	if ! [ -d "./build" ]; then \
        mkdir build; \
    fi
	cp ./*.o ../../build
	mv ./*.o ./build

compile_platform: $(SRC)
	$(CC) $(CFLAGS) $(INC) $^ -c 

clean:
	rm -f ./build/*
