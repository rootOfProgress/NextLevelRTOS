CC := arm-none-eabi-gcc
AS := arm-none-eabi-as
NON_OS_SRC := ../modules
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

# $(TARGET): 	clean \
# 			prepare_environment \
# 			build_non_os_src \
# 			build_base_kernel \
# 			build_platform_$(ARCH) \
# 			link_objects \

$(TARGET): 	clean \
			prepare_environment \
			build_non_os_src \
			build_base_kernel \
			compile_asm \
			build_platform_$(ARCH) \
			link_objects \
			clean_objects

ifeq (1,$(SYSTICK))
CFLAGS += -DSYSTICK=1
else
CFLAGS += -DSYSTICK=0
endif

ifeq (1,$(DEBUG))
CFLAGS += -DDEBUG=1
else ifeq (2,$(DEBUG))
CFLAGS += -DDEBUG=2
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
CFLAGS += -DGIT_HASH_PARSED="0x"$(shell git rev-parse --short HEAD )

export CFLAGS

build_platform_$(ARCH):
	make -C arch/$(ARCH) HWFP=$(HWFP)

build_non_os_src:
# stage1: check for necessary modules
	python $(NON_OS_SRC)/collect_activated_modules.py $(CURDIR)
# stage2: build sources from results
	make -C $(NON_OS_SRC)

prepare_environment:
	@echo "Prepare Environment..."
	mkdir -p ./build

build_base_kernel: $(SRC)
	@echo "Build C Sources..."
	$(CC) $(CFLAGS) $(INC) -c $^ 
	mv *.o ./build

compile_asm: $(SRC_AS)
	@echo "Compile Assembly Sources..."
	$(AS) $^ -o ./build/asm.o

link_objects: 
	@echo "Execute Linker Stage..."
	make -C ./buildscripts TARGET=$(ARCH)

clean:
	rm -f ./build/*.o ./build/*.$(ARCH) ./build/memmap.$(ARCH)

clean_objects:
	rm -f ./build/*.o