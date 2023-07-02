CC := arm-none-eabi-gcc
AS := arm-none-eabi-as
OBJCP := arm-none-eabi-objcopy
INC = -I../../packages/lib/nrf2401/include -I../../packages/lib/am2302/include -Iinclude -I../../os/arch/$(ARCH)/include/ -I../include
OS_BUILD_DIR = ../../os/build
OUTDIR := ./build

CFLAGS = -O0  \
	 -g -mcpu=cortex-m4 -mthumb \
	 -nostartfiles -ffunction-sections  -Wall -Wextra -mfloat-abi=soft -static -nostdlib  \

SRC:=$(wildcard ./src/**/*.c) $(wildcard ./src/*.c)
OBJ:=$(wildcard ./build/*.o)

TARGET := weathernode

all: $(TARGET)

$(TARGET): clean compile_sources

compile_sources: $(SRC)
	$(CC) $(CFLAGS) $(INC) -c $^ 
	mv *.o $(OS_BUILD_DIR)

link_objects: 
	make -C ./buildscripts

clean:
	rm -f $(OUTDIR)/*.o $(OUTDIR)/$(TARGET)
