CC := arm-none-eabi-gcc
AS := arm-none-eabi-as
NON_OS_SRC := ../drohnentasks
INC = -Iinclude -I$(NON_OS_SRC)/include/ -I./arch/$(ARCH)/include/
LINK = link.ld

CFLAGS = -O3  \
	 -mcpu=cortex-m4 -mthumb \
	 -g -nostartfiles -ffreestanding -ffunction-sections -Wall -Wextra -static \
	 -Wno-implicit-fallthrough -fsingle-precision-constant 

SRC:=$(wildcard ./src/**/*.c) $(wildcard ./src/*.c)
OBJ:=$(wildcard ./build/*.o)

SRC_AS=$(wildcard ./src/**/*.s) $(wildcard ./src/*.s)
TARGET = os
all: $(TARGET)

$(TARGET): clean build_kernel compile_as build_platform_$(ARCH) link_objects clean_objects

ifeq (1,$(SYSTICK))
CFLAGS += -DSYSTICK=1
else
CFLAGS += -DSYSTICK=0
endif

ifeq (1,$(DEBUG))
CFLAGS += -DDEBUG=1
else
CFLAGS += -DDEBUG=0
endif

ifeq (1,$(TEST))
CFLAGS += -DTEST=1
else
CFLAGS += -DTEST=0
endif

ifeq (1,$(HWFP))
CFLAGS += -mfloat-abi=hard
CFLAGS += -mfpu=fpv4-sp-d16
CFLAGS += -DHWFP=1
else
CFLAGS += -mfloat-abi=soft
CFLAGS += -DHWFP=0
endif

CFLAGS += -DARCH

export CFLAGS

build_platform_$(ARCH):
	make -C arch/$(ARCH) HWFP=$(HWFP)

build_non_os_src:
	make -C $(NON_OS_SRC) HWFP=$(HWFP)

move_objects:
	mv *.o ./build

build_kernel: $(SRC)
	$(CC) $(CFLAGS) $(INC) -c $^ 
	mv *.o ./build

compile_as: $(SRC_AS)
	$(AS) $^ -o ./build/asm.o

link_objects: 
	make -C ./buildscripts


clean:
	rm -f ./build/*.o *.$(ARCH) memmap.$(ARCH)

clean_objects:
	rm -f ./build/*.o