CC := arm-none-eabi-gcc
AS := arm-none-eabi-as
OBJCP := arm-none-eabi-objcopy
INC = -Iinclude -I../../os/arch/$(ARCH)/include/ -I../../packages/lib/include -I../../os/include/shared
OS_BUILD_DIR = ../../os/build
OUTDIR := ./build

CFLAGS = -O0  \
	 -g -mcpu=cortex-m4 -mthumb \
	 -nostartfiles -ffunction-sections  -Wall -Wextra -mfloat-abi=soft -static -nostdlib  \

SRC:=$(wildcard ./src/**/*.c) $(wildcard ./src/*.c)
OBJ:=$(wildcard ./build/*.o)

TARGET := dummy

all: $(TARGET)

$(TARGET): clean compile_sources

compile_sources: $(SRC)
	$(CC) $(CFLAGS) $(INC) -c $^ 
	mv *.o $(OS_BUILD_DIR)

link_objects: 
	make -C ./buildscripts

clean:
	rm -f $(OUTDIR)/*.o ./build/$(TARGET)*
