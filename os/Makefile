CC := arm-none-eabi-gcc
AS := arm-none-eabi-as
NON_OS_SRC := ../drohnentasks
INC = -Iinclude -I$(NON_OS_SRC)/include/
LINK = link.ld

CFLAGS = -O2  \
	 -mcpu=cortex-m4 -mthumb \
	 -nostartfiles -ffreestanding -ffunction-sections -Wall -Wextra -static \
	 -g -Wno-implicit-fallthrough -fsingle-precision-constant 
	 
#-print-libgcc-file-name \
#-print-multiarch 

SRC:=$(wildcard ./src/**/*.c) $(wildcard ./src/*.c)
OBJ:=$(wildcard ./build/*.o)

SRC_AS=$(wildcard ./src/**/*.s) $(wildcard ./src/*.s)
TARGET = firmware
all: $(TARGET)

$(TARGET): clean build_kernel compile_as build_platform_$(ARCH) build_non_os_src link_objects

ifeq (1,$(SYSTICK))
CFLAGS += -DSYSTICK=1
else
CFLAGS += -DSYSTICK=0
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
#/sARCH=$(ARCH) 

clean:
	rm -f ./build/*.o $(TARGET)
