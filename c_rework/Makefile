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
# ARCH = $()
all: $(TARGET)



$(TARGET): clean compile_c compile_as $(ARCH) link

# # make_arch:
# ifneq ($(ARCH),)
# 	make -C arch/$(ARCH)
# else
# 	echo "No Device Selected, Building Standalone Kernel"
# endif

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
